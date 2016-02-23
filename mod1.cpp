#include "Map.class.hpp"
#include "general.hpp"

bool skip_frames = true;

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
