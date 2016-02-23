#include "Map.class.hpp"
#define RENDER_AHEAD 30
#define FPS 30

bool skip_frames = true;
std::queue<Map> q;
std::mutex mtx;

void console_main(void)
{
	while (1)
	{
		usleep(1000000 / FPS);
		mtx.lock();
		if (q.size() < 1)
		{
			mtx.unlock();
			continue ;
		}
		mtx.unlock();
		// system("clear");
		if (skip_frames)
			std::cout << std::endl << q.back().to_string() << std::endl;
		else
			std::cout << std::endl << q.front().to_string() << std::endl;
		mtx.lock();
		q.pop();
		mtx.unlock();
	}
}

int main(int ac, char const *av[])
{
	Map map(200, 50);

	// Generer le terrain
	map.elevate_rect(5, 5, 5 + 20, 7 + 10, -50);
	map.viscosity = 0;
	std::cout << "started" << std::endl;
	map.elevate_rect(20, 0, 20 + 3, map.height, 85);
	for (int y = 0; y < map.height - 4; y++)
	{
		map.elevate_rect(map.width / 2 - 6, 0, map.width / 2 - 5, y, 100);
	}
	// map.drop_water(10, 10, 40000);
	// map.drop_water(map.width / 2, map.height / 2, 400000);

	// La vague :
	for (int y = 0; y < map.height; y++)
	{
		map.drop_water(map.width - 1, y, map.width * map.height);
	}

	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() >= RENDER_AHEAD)
			{
				mtx.unlock();
				usleep(100000);
				continue ;
			}
			mtx.unlock();
			// map.drop_water(0, 0, 100);
			map.apply_gravity();
			mtx.lock();
			q.push(map);
			mtx.unlock();
		}
	});
	console_main();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
