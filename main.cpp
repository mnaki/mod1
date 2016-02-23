#include "Affichage.hpp"
#include "general.hpp"
#include <GLUT/glut.h>

std::queue<Map> q;
std::mutex mtx;

int	main(int ac, char **av)
{
	glutInit(&ac, av);
	glutInitWindowSize(W_Width, W_Height);
	glutCreateWindow("single triangle");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	Map map(200, 50);
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
		map.drop_water(map.width - 1, y, map.width * map.height * 2);
	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() >= RENDER_AHEAD)
			{
				mtx.unlock();
				continue ;
			}
			mtx.unlock();
			// map.drop_water(0, 0, 100);
			map.apply_gravity();
			mtx.lock();
			q.push(map);
			mtx.unlock();
			glutPostRedisplay();
		}
	});
 	glutMainLoop();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
