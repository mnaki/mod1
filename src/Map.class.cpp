#include <limits>
#include <cstddef>
#include <random>
#include <array>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include "Map.class.hpp"
#include "general.hpp"

void Map::update_rain(void)
{
    auto new_end = std::remove_if(rain_drops.begin(), rain_drops.end(), [this](RainDrop & drop)
    {
        if (drop.x <= 1 || drop.y <= 1 || drop.x >= width - 1 - 1 || drop.y >= height - 1 - 1)
        {
            return true;
        }
        else if (drop.altitude > data[drop.x][drop.y].terrain_height + data[drop.x][drop.y].water_level - 1)
        {
            if (drop.is_snow)
            {
                drop.altitude -= drop.mass * 7 * drop.mass * 7;
                float mouvement = 2.0f * drop.mass;
                if (rand() == 0)
                    drop.x += mouvement;
                if (rand() == 0)
                    drop.y += mouvement;
            }
            else
            {
                drop.altitude -= drop.mass * 10.0f * drop.mass * 10.0f;
            }
            return false;
        }
        else
        {
            drop_water(drop.x, drop.y, drop.mass);
            return true;
        }
        return true;
    });
    rain_drops.erase(new_end, rain_drops.end());
}

void Map::drop_snow(int x, int y, float mass)
{
    RainDrop drop;
    drop.x = x;
    drop.y = y;
    drop.altitude = data[drop.x][drop.y].terrain_height + data[drop.x][drop.y].water_level + width;
    drop.mass = mass;
    drop.is_snow = true;
    rain_drops.push_back(drop);
}

void Map::drop_rain(int x, int y, float mass)
{
    RainDrop drop;
    drop.x = x;
    drop.y = y;
    drop.altitude = data[drop.x][drop.y].terrain_height + data[drop.x][drop.y].water_level + width;
    drop.mass = mass;
    rain_drops.push_back(drop);
}

void Map::drop_water(int x, int y, float quantity)
{
	this->data[x][y].water_level = this->data[x][y].water_level + quantity;
}

void Map::set_water(int x, int y, float value)
{
	this->data[x][y].water_level = value;
}

Map::Map(int width, int height, int scenario) : width(width), height(height), scenario(scenario)
{
	this->data.resize(width);
	for (int x = 0 ; x < width ; x++)
	{
		this->data[x].resize(height);
	}
}

int Map::get_hauteur_max(void) const
{
	int res = 0;
	for (int i = 0 ; i < width ; i++)
		for (int j = 0 ; j < height ; j++)
			res = (res > data[i][j].terrain_height) ? res : (int)data[i][j].terrain_height;
	return res;
}

float Map::resistance(int x, int y)
{
	if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1)
		return resistance(this->data[x][y]);
	else
		return std::numeric_limits<float>::max();
}

float Map::resistance(MapPoint const & point) {return point.water_level.load() + (float)point.terrain_height.load();}

typedef struct toto
{
	int i;
	int j;
} tata;

void Map::apply_gravity(void)
{
	std::vector<std::thread> threads(CONF_NUM_CORES);
	static std::random_device rd;
	static std::mt19937 g(rd());

    update_rain();
    try
    {
		for (int thread_id = 0 ; thread_id < CONF_NUM_CORES ; thread_id++)
		{
			threads[thread_id] = std::thread([this, thread_id]()
			{
				#ifdef __LINUX__
				{
					cpu_set_t cpuset;
					pthread_t thread;
					thread = pthread_self();
					CPU_ZERO(&cpuset);
					CPU_SET(thread_id, &cpuset);
					pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
				}
				#endif
				for (int x = thread_id * (this->width / CONF_NUM_CORES) ; x < (thread_id+1.0) * (this->width / CONF_NUM_CORES) ; x++)
				{
					std::vector<tata*> points;
					for (int i = 1 ; i <= 1 ; i++)
					{
						tata *a;
						a = new tata();	a->i = i;	a->j = 0;	points.push_back(a);
						a = new tata();	a->i = 0;	a->j = i;	points.push_back(a);
						a = new tata();	a->i = -i;	a->j = 0;	points.push_back(a);
						a = new tata();	a->i = 0;	a->j = -i;	points.push_back(a);

						a = new tata();	a->i = -i;	a->j = -i;	points.push_back(a);
						a = new tata();	a->i = i;	a->j = i;	points.push_back(a);
						a = new tata();	a->i = -i;	a->j = i;	points.push_back(a);
						a = new tata();	a->i = i;	a->j = -i;	points.push_back(a);
					}

					for (int y = 0 ; y < this->height ; y++)
					{
						float old_val = 0;
						while (this->data[x][y].water_level > 0 && this->data[x][y].water_level != old_val)
						{
							old_val = this->data[x][y].water_level;
							std::shuffle(points.begin(), points.end(), g);
							for (float pression = 1 ; pression < this->data[x][y].water_level + 1.1f ; pression++)
							{
								for (auto iter = points.begin(); iter != points.end(); ++iter)
									ecoulement_unitaire(x, y, (*iter)->i * pression, (*iter)->j * pression);
							}
						}
					}
				}
			});
		}
		for (int thread_id = 0 ; thread_id < CONF_NUM_CORES ; thread_id++)
		{
			threads[thread_id].join();
		}
	}
	catch (std::exception const & e)
	{
		std::cout << e.what() << std::endl;
	}
}

void	Map::ecoulement_unitaire(int x, int y, int i, int j)
{
	if ((x+i >= conf_marge_bocal && x+i < width - conf_marge_bocal - 1 && y+j >= conf_marge_bocal && y+j < height - conf_marge_bocal - 1)
			&& resistance(this->data[x+i][y+j]) < resistance(x, y))
	{
		this->data[x][y].water_level = this->data[x][y].water_level - 0.1f;
		this->data[x+i][y+j].water_level = this->data[x+i][y+j].water_level + 0.1f;
	}
}
std::string Map::to_string(void) const
{
	std::stringstream ss;
	for (int y = 0 ; y < this->height ; y++)
	{
		for (int x = 0 ; x < this->width ; x++)
		{
			std::string color;
			if (this->data[x][y].water_level == 0)
			{
				color = GREEN;
				if (this->data[x][y].terrain_height > 10)
					color = WHITE;
				if (this->data[x][y].terrain_height < 0)
					color = YELLOW;
			}
			if (this->data[x][y].water_level > 0)
				color = CYAN;
			if (this->data[x][y].water_level > 20)
				color = BLUE;
			if (this->data[x][y].water_level > 35)
				color = BLACK;
			ss << color << this->data[x][y].water_level + this->data[x][y].terrain_height << std::string(" ");
		}
		ss << std::endl;
	}
	return ss.str();
}

void Map::elevate_rect(int x0, int y0, int x1, int y1, float value)
{
	int width = x1 - x0;
	int height = y1 - y0;
	for (int x = 0 ; x < width ; x++)
	for (int y = 0 ; y < height ; y++)
		this->data[x+x0][y+y0].terrain_height = this->data[x+x0][y+y0].terrain_height + value;
}

void Map::draw_cone(int start_x, int start_y, int radius, int height, bool reverse)
{
	float r = radius;
	for (float h = 0 ; h < height ; h += 1.0f)
	{
		r *= 1.0f - (h / height) / 100.0f;
		for (float x = 0 ; x < this->width ; x += 1.0f)
		for (float y = 0 ; y < this->height ; y += 1.0f)
		{
			if (((x - start_x) * (x - start_x) + (y - start_y) * (y - start_y)) <= (r) * (r))
			{
				if (reverse)
					this->data[x][y].terrain_height = this->data[x][y].terrain_height - h/1000.0f;
				else
					this->data[x][y].terrain_height = this->data[x][y].terrain_height + h/1000.0f;
			}
		}
	}
}
