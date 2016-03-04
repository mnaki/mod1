#include "Map.class.hpp"
#include <limits>
#include <cstddef>
#include <random>
#include <array>

void Map::drop_water(int x, int y, int quantity)
{
	this->data[x][y].water_level += quantity;
}

Map::Map(int width, int height, std::string scenario) : width(width), height(height), scenario(scenario)
{
	this->data.resize(width);
	for (int x = 0 ; x < width ; x++)
	{
		this->data[x].resize(height);
	}
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
		if (lhs == NULL)
			return false;
		if (rhs == NULL)
			return false;
		return lhs->water_level.load() + lhs->terrain_height.load() < rhs->water_level.load() + rhs->terrain_height.load();
	}
};

void Map::apply_gravity(void)
{
	std::thread threads[(int)MAX_THREAD_COUNT];
	static std::array<MapPoint*,8*8> points[(int)MAX_THREAD_COUNT];

	for (int thread_id = 0 ; thread_id < MAX_THREAD_COUNT ; thread_id++)
	{
		threads[thread_id] = std::thread([this, thread_id](){
			std::random_device rd;
			std::mt19937 g(rd());
			for (int x = thread_id * (this->width / MAX_THREAD_COUNT) ; x < (thread_id+1.0) * (this->width / MAX_THREAD_COUNT) ; x++)
			{
				for (int y = 0 ; y < this->height ; y++)
				{
					if (this->data[x][y].water_level > 1)
					{
						points[thread_id].fill(NULL);
						int k = 0;
						for (int i = 1; i <= 1; i++)
						{
							if ((x+i >= 0 && x+i < width - 1 && y >= 0 && y < height - 1) && resistance(this->data[x+i][y]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x+i][y];
							if ((x >= 0 && x < width - 1 && y+i >= 0 && y+i < height - 1) && resistance(this->data[x][y+i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x][y+i];
							if ((x-i >= 0 && x-i < width - 1 && y >= 0 && y < height - 1) && resistance(this->data[x-i][y]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x-i][y];
							if ((x >= 0 && x < width - 1 && y-i >= 0 && y-i < height - 1) && resistance(this->data[x][y-i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x][y-i];

							if ((x-i >= 0 && x-i < width - 1 && y-i >= 0 && y-i < height - 1) && resistance(this->data[x-i][y-i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x-i][y-i];
							if ((x+i >= 0 && x+i < width - 1 && y+i >= 0 && y+i < height - 1) && resistance(this->data[x+i][y+i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x+i][y+i];
							if ((x-i >= 0 && x-i < width - 1 && y+i >= 0 && y+i < height - 1) && resistance(this->data[x-i][y+i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x-i][y+i];
							if ((x+i >= 0 && x+i < width - 1 && y-i >= 0 && y-i < height - 1) && resistance(this->data[x+i][y-i]) < resistance(x, y))
								points[thread_id][k++] = &this->data[x+i][y-i];
						}
						if (points[thread_id].size() == 0)
							continue;
						std::shuffle(points[thread_id].begin(), points[thread_id].end(), g);
						std::sort(std::begin(points[thread_id]), std::end(points[thread_id]), compare_points());
						for (MapPoint* point : points[thread_id])
						{
							if (point == NULL)
								continue ;
							int r = resistance(*point) < resistance(x, y);
							this->data[x][y].water_level -= r;
							point->water_level += r;
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

void Map::elevate_rect(int x0, int y0, int x1, int y1, int value)
{
	int width = x1 - x0;
	int height = y1 - y0;
	for (int x = 0 ; x < width ; x++)
	for (int y = 0 ; y < height ; y++)
		this->data[x+x0][y+y0].terrain_height += value;
}

void Map::draw_cone(int start_x, int start_y, int radius, int height, bool reverse)
{
	double r = radius;
	for (double h = 0 ; h < height ; h += 10.0)
	{
		r *= 1.0f - (h / height) / 20.0f;
		for (double x = 0 ; x < this->width ; x += 1.0)
		for (double y = 0 ; y < this->height ; y += 1.0)
		{
			if (((x - start_x) * (x - start_x) + (y - start_y) * (y - start_y)) <= (r) * (r))
			{
				if (reverse)
					this->data[x][y].terrain_height -= h/100;
				else
					this->data[x][y].terrain_height += h/100;
			}
		}
	}
}
