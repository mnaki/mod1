#include "Affichage.hpp"
#include "general.hpp"
#ifdef __LINUX__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

std::queue<Map> q;
std::mutex mtx;

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(80, 60);
	map.elevate_rect(5, 5, 5 + 20, 7 + 10, -50);
	map.viscosity = 0;
	std::cout << "started" << std::endl;
	map.elevate_rect(20, 0, 20 + 3, map.height, 85);
	for (int y = 0; y < map.height - 4; y++)
	{
		map.elevate_rect(map.width / 2 - 6, 0, map.width / 2 - 5, y, 100);
	}

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map.width * LARGEUR_PIXEL) ? W_Width : map.width * LARGEUR_PIXEL;
	h = (W_Height > map.height * LONGEUR_PIXEL) ? W_Height : map.height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("single triangle");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	// map.drop_water(10, 10, 40000);
	// map.drop_water(map.width / 2, map.height / 2, 400000);

	// La vague :
	for (int y = 0; y < map.height; y++)
		map.drop_water(map.width - 1, y, map.width * map.height);
	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() >= RENDER_AHEAD)
			{
				mtx.unlock();
				usleep(10000);
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
