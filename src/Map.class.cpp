#include "Map.class.hpp"
#include <limits>
#include <cstddef>
#include <random>
#include <array>

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

int Map::resistance(int x, int y)
{
	if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1)
	{
		return this->data[x][y].water_level.load() + this->data[x][y].terrain_height.load();
	}
	else
	{
		return std::numeric_limits<int>::max();
	}
}

int Map::resistance(MapPoint const & point)
{
	return point.water_level.load() + point.terrain_height.load();
}

struct compare_points
{
	inline bool operator() (const MapPoint * lhs, const MapPoint * rhs)
	{
		if (lhs == NULL && rhs != NULL)
			return false;
		else if (lhs != NULL && rhs == NULL)
			return true;
		else if (lhs == NULL && rhs == NULL)
			return false;
		return lhs->water_level.load() + lhs->terrain_height.load() <= rhs->water_level.load() + rhs->terrain_height.load();
	}
};

#define MAX_LOOKUP_DISTANCE 2

void Map::apply_gravity(void)
{
	std::thread threads[(int)MAX_THREAD_COUNT];
	static std::array<MapPoint*,MAX_LOOKUP_DISTANCE*8> points[(int)MAX_THREAD_COUNT];
	static std::random_device rd;
	static std::mt19937 g(rd());

	for (int thread_id = 0 ; thread_id < MAX_THREAD_COUNT ; thread_id++)
	{
		threads[thread_id] = std::thread([this, thread_id](){
			for (int x = thread_id * (this->width / MAX_THREAD_COUNT) ; x < (thread_id+1.0) * (this->width / MAX_THREAD_COUNT) ; x++)
			{
				for (int y = 0 ; y < this->height ; y++)
				{
					float k = points[thread_id].size() - 1;
					for (float l = 0; l < k; l++)
					{
						points[thread_id].fill(NULL);
						k = 0;
						points[thread_id][k++] = &this->data[x][y];
						for (int i = 1; i <= MAX_LOOKUP_DISTANCE; i++) // 'i' etant la distance maximale de case adjacente a verifier
						{
							if ((x+i >= 0 + conf_marge_bocal && x+i < width - (1 + conf_marge_bocal) && y >= 0 + conf_marge_bocal && y < height - (1 + conf_marge_bocal)) && resistance(this->data[x+i][y]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x+i][y];
							if ((x >= 0 + conf_marge_bocal && x < width - (1 + conf_marge_bocal) && y+i >= 0 + conf_marge_bocal && y+i < height - (1 + conf_marge_bocal)) && resistance(this->data[x][y+i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x][y+i];
							if ((x-i >= 0 + conf_marge_bocal && x-i < width - (1 + conf_marge_bocal) && y >= 0 + conf_marge_bocal && y < height - (1 + conf_marge_bocal)) && resistance(this->data[x-i][y]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x-i][y];
							if ((x >= 0 + conf_marge_bocal && x < width - (1 + conf_marge_bocal) && y-i >= 0 + conf_marge_bocal && y-i < height - (1 + conf_marge_bocal)) && resistance(this->data[x][y-i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x][y-i];

							if ((x-i >= 0 + conf_marge_bocal && x-i < width - (1 + conf_marge_bocal) && y-i >= 0 + conf_marge_bocal && y-i < height - (1 + conf_marge_bocal)) && resistance(this->data[x-i][y-i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x-i][y-i];
							if ((x+i >= 0 + conf_marge_bocal && x+i < width - (1 + conf_marge_bocal) && y+i >= 0 + conf_marge_bocal && y+i < height - (1 + conf_marge_bocal)) && resistance(this->data[x+i][y+i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x+i][y+i];
							if ((x-i >= 0 + conf_marge_bocal && x-i < width - (1 + conf_marge_bocal) && y+i >= 0 + conf_marge_bocal && y+i < height - (1 + conf_marge_bocal)) && resistance(this->data[x-i][y+i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x-i][y+i];
							if ((x+i >= 0 + conf_marge_bocal && x+i < width - (1 + conf_marge_bocal) && y-i >= 0 + conf_marge_bocal && y-i < height - (1 + conf_marge_bocal)) && resistance(this->data[x+i][y-i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x+i][y-i];
						}
						// std::shuffle(points[thread_id].begin(), points[thread_id].end(), g);
						// std::sort(std::begin(points[thread_id]), std::end(points[thread_id]), compare_points());
						// std::reverse(std::begin(points[thread_id]), std::end(points[thread_id]));
						for (MapPoint* point : points[thread_id])
						{
							if (point != NULL && this->data[x][y].water_level > 0.0f && resistance(*point) < resistance(this->data[x][y]))
							{
								this->data[x][y].water_level = this->data[x][y].water_level - 0.01f * k;
								point->water_level = point->water_level + 0.01f * k;
							}
						}
					}
				}
			}
		});
	}
	for (int thread_id = 0 ; thread_id < MAX_THREAD_COUNT ; thread_id++)
	{
		threads[thread_id].join();
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
