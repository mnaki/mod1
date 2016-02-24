#include "Map.class.hpp"

void Map::drop_water(int x, int y, int quantity)
{
	this->data[x][y].water_level += quantity;
}

Map::Map(int width, int height) : width(width), height(height)
{
	this->data.resize(width);
	for (int x = 0; x < width; x++)
	{
		this->data[x].resize(height);
	}
}

void Map::apply_gravity(void)
{
	std::thread * threads[MAX_THREAD_COUNT];

	for (int thread_id = 0; thread_id < MAX_THREAD_COUNT; thread_id++)
	{
		threads[thread_id] = new std::thread([this, thread_id](){
			std::vector<MapPoint*> neighbours;
			neighbours.reserve(9);
			for (int x = 0; x < this->width; x++)
			{
				for (int y = (double)thread_id * ((double)this->height / (double)MAX_THREAD_COUNT); (double)y < (double)((double)thread_id+1.0) * (double)((double)this->height / (double)MAX_THREAD_COUNT); y++)
				{
					// this->data[x][y].terrain_height = 10000000;
					for (int l = 0; l < this->data[x][y].water_level * (1.0 - this->viscosity); l++)
					{
						this->data[x][y].water_level--;
						neighbours.clear();
						neighbours.push_back(&this->data[x][y]);
						for (int i = 1; i <= 1; i++)
						{
							// int i = 1;
							if (x < this->width - i)
								neighbours.push_back(&this->data[x+i][y]);
							if (x >= i)
								neighbours.push_back(&this->data[x-i][y]);
							if (y < this->height - i)
								neighbours.push_back(&this->data[x][y+i]);
							if (y >= i)
								neighbours.push_back(&this->data[x][y-i]);

							// diagonales
							// a voir

							// if (x < this->width - i && y < this->height - i)
							// 	neighbours.push_back(&this->data[x+i][y+i]);
							// if (x >= i && y < this->height - i)
							// 	neighbours.push_back(&this->data[x-i][y+i]);
							// if (x >= i && y >= i)
							// 	neighbours.push_back(&this->data[x-i][y-i]);
							// if (y >= i && x < this->width - i)
							// 	neighbours.push_back(&this->data[x+i][y-i]);
						}
						MapPoint * map_point = neighbours[0];
						for (MapPoint * n : neighbours)
						{
							if (n->terrain_height + n->water_level < map_point->terrain_height + map_point->water_level)
							{
								map_point = n;
							}
						}
						map_point->water_level++;
					}
					sleep(0);
				}
			}
		});
	}
	for (int thread_id = 0; thread_id < MAX_THREAD_COUNT; thread_id++)
	{
		threads[thread_id]->join();
		delete threads[thread_id];
	}
}

std::string Map::to_string(void) const
{
	std::stringstream ss;
	for (int y = 0; y < this->height; y++)
	{
		for (int x = 0; x < this->width; x++)
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
			ss << color << "█";
			// ss << color << this->data[x][y].water_level + this->data[x][y].terrain_height << " ";
		}
		ss << std::endl;
	}
	return ss.str();
}

void Map::elevate_rect(int x0, int y0, int x1, int y1, int value)
{
	int width = x1 - x0;
	int height = y1 - y0;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			this->data[x+x0][y+y0].terrain_height += value;
		}
	}
}
