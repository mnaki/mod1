#include "Map.class.hpp"

void Map::drop_water(int x, int y, int quantity)
{
	this->data[x][y].water_level += quantity;
}

Map::Map(int width, int height) : width(width), height(height)
{
	this->data.resize(width);
	for (int x = 0 ; x < width ; x++)
	{
		this->data[x].resize(height);
	}
}

void Map::apply_gravity(void)
{
	std::thread * threads[MAX_THREAD_COUNT];

	for (int thread_id = 0 ; thread_id < MAX_THREAD_COUNT ; thread_id++)
	{
		threads[thread_id] = new std::thread([this, thread_id](){
			for (int x = thread_id * (this->width / MAX_THREAD_COUNT) ; x < (thread_id+1.0) * (this->width / MAX_THREAD_COUNT) ; x++)
			{
				for (int y = 0 ; y < this->height ; y++)
				{
					for (int l = 0 ; l < this->data[x][y].water_level * (1.0 - this->viscosity) ; l++)
					{
						MapPoint* neighbours[8] = { NULL };
						int i = 1;
						if (x < this->width - i)
							neighbours[0] = (&this->data[x+i][y]);
						if (x > i - 1)
							neighbours[1] = (&this->data[x-i][y]);
						if (y < this->height - i)
							neighbours[2] = (&this->data[x][y+i]);
						if (y > i - 1)
							neighbours[3] = (&this->data[x][y-i]);

						if (x < this->width - i && y < this->height - i)
							neighbours[4] = (&this->data[x+i][y+i]);
						if (x > i - 1 && y > i - 1)
							neighbours[5] = (&this->data[x-i][y-i]);
						if (y < this->height - i && x > i - 1)
							neighbours[6] = (&this->data[x-i][y+i]);
						if (y > i - 1 && x < this->width - i)
							neighbours[7] = (&this->data[x+i][y-i]);

						MapPoint * map_point = (&this->data[x][y]);
						for (int i = 0 ; i < 8 ; i++)
						{
							if (neighbours[i] != NULL && neighbours[i]->terrain_height.load() + neighbours[i]->water_level.load() < map_point->terrain_height.load() + map_point->water_level.load())
							{
								map_point = neighbours[i];
							}
						}
						// if (this->data[x][y].water_level >= 1)
						// {
							this->data[x][y].water_level -= 1;
							map_point->water_level += 1;
						// }
					}
				}
			}
		});
	}
	for (int thread_id = 0 ; thread_id < MAX_THREAD_COUNT ; thread_id++)
	{
		threads[thread_id]->join();
		delete threads[thread_id];
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
	for (int x = 0 ; x < width ; x++)
	for (int y = 0 ; y < height ; y++)
		this->data[x+x0][y+y0].terrain_height += value;
}

void Map::draw_cone(int start_x, int start_y, int radius, int height, bool reverse)
{
	double r = radius;
	for (double h = 0 ; h < height ; h += 10.0)
	{
		r *= 1.0 - (h / height) / 50.0;
		for (double x = 0 ; x < this->width ; x += 1.0)
		for (double y = 0 ; y < this->height ; y += 1.0)
		{
			if (((x - start_x) * (x - start_x) + (y - start_y) * (y - start_y)) <= (r) * (r))
			{
				if (reverse)
					this->data[x][y].terrain_height -= 1;
				else
					this->data[x][y].terrain_height += 1;
			}
		}
	}
}
