#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <atomic>
#include <mutex>

struct
MapPoint
{
	std::atomic<int> terrain_height;
	std::atomic<int> water_level;
	MapPoint();
	MapPoint(const MapPoint & rhs);
	~MapPoint();
	MapPoint & operator=(const MapPoint & rhs);
};



MapPoint::MapPoint() : terrain_height(0), water_level(0)
{}

MapPoint::MapPoint(const MapPoint & rhs)
{
	terrain_height = rhs.terrain_height.load();
	water_level = rhs.water_level.load();
}

MapPoint::~MapPoint()
{}

MapPoint & MapPoint::operator=(const MapPoint & rhs)
{
	std::memcpy(this, &rhs, sizeof(*this));
	return *this;
}

class
Map
{

public:

	std::vector< std::vector<MapPoint> > data;
	int const width;
	int const height;
	double viscosity = 0.0; // 0.0 = Liquide ; 1.0 = Solid

	Map(int width = 1, int height = 1);
	~Map() = default;
	void drop_water(int x, int y, int quantity);
	void apply_gravity(void);
	void elevate_rect(int x0, int y0, int x1, int y1, int value);
	std::string to_string(void) const;
};

Map::Map(int width, int height) : width(width), height(height)
{
	this->data.resize(width);
	for (int x = 0; x < width; x++)
	{
		this->data[x].resize(height);
	}
}

void
Map::drop_water(int x, int y, int quantity)
{
	this->data[x][y].water_level += quantity;
}

# define MAX_THREAD_COUNT (100)
# include <thread>

void
Map::apply_gravity(void)
{
	std::thread * threads[MAX_THREAD_COUNT];

	for (int thread_id = 0; thread_id < MAX_THREAD_COUNT; thread_id++)
	{
		threads[thread_id] = new std::thread([this, thread_id](){
			std::vector<MapPoint*> neighbours;
			neighbours.reserve(9);
			for (int y = (double)thread_id * ((double)this->height / (double)MAX_THREAD_COUNT); (double)y < (double)((double)thread_id+1.0) * (double)((double)this->height / (double)MAX_THREAD_COUNT); y++)
			{
				for (int x = 0; x < this->width; x++)
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
							if (y < this->height - i)
								neighbours.push_back(&this->data[x][y+i]);
							if (x >= i)
								neighbours.push_back(&this->data[x-i][y]);
							if (y >= i)
								neighbours.push_back(&this->data[x][y-i]);
							if (x < this->width - i && y < this->height - i)
								neighbours.push_back(&this->data[x+i][y+i]);
							if (x >= i && y < this->height - i)
								neighbours.push_back(&this->data[x-i][y+i]);
							if (y >= i && x < this->width - i)
								neighbours.push_back(&this->data[x+i][y-i]);
							if (x >= i && y >= i)
								neighbours.push_back(&this->data[x-i][y-i]);
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
				}
			}
		});
	}
	for (int thread_id = 0; thread_id < MAX_THREAD_COUNT; thread_id++)
	{
		threads[thread_id]->join();
		delete threads[thread_id];
	}
	// delete [] threads;
}

# define YELLOW "\033[1;33m"
# define WHITE "\033[1;37m"
# define BLACK "\033[1;30m"
# define GREEN "\033[1;32m"
# define CYAN "\033[1;36m"
# define BLUE "\033[1;34m"

std::string
Map::to_string(void) const
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

void
Map::elevate_rect(int x0, int y0, int x1, int y1, int value)
{
	int width = x1 - x0;
	int height = y1 - y0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			this->data[x+x0][y+y0].terrain_height += value;
		}
	}
}

# define RENDER_AHEAD 100

int
main(int ac, char const *av[])
{
	std::queue<Map> q;
	std::mutex mtx;
	Map map(100, 40);
	map.elevate_rect(5, 5, 5 + 20, 7 + 10, -50);
	map.viscosity = 0;
	std::cout << "started" << std::endl;
	map.elevate_rect(20, 0, 20 + 3, map.height, 85);
	map.elevate_rect(1, map.height - 20, 15, map.height - 1, -60);
	map.drop_water(map.width / 2, map.height / 2, 180000);
	std::thread t([&q, &map, &mtx]{
		while (1)
		{
			mtx.lock();
			if (q.size() >= RENDER_AHEAD)
			{
				mtx.unlock();
				continue ;
			}
			mtx.unlock();
			// map.drop_water(0, 0, 1000);
			map.apply_gravity();
			mtx.lock();
			q.push(map);
			mtx.unlock();
		}
	});
	while (1)
	{
		// sleep(1 / 3);
		mtx.lock();
		if (q.size() < 1)
		{
			mtx.unlock();
			continue ;
		}
		mtx.unlock();
		system("clear");
		std::cout << std::endl << q.front().to_string() << std::endl;
		mtx.lock();
		q.pop();
		mtx.unlock();
	}
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
