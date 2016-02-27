#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(200, 150);
	map.elevate_rect(5, 5, 5 + 20, 7 + 10, -5);
	map.viscosity = 0.9;
	std::cout << "started" << std::endl;
	map.elevate_rect(20, 0, 20 + 3, map.height, 1);
	for (int y = 0; y < map.height - 4; y++)
	{
		map.elevate_rect(map.width / 2 - 6, 10, map.width / 2 - 5, y, 2);
	}

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map.width * LARGEUR_PIXEL) ? W_Width : map.width * LARGEUR_PIXEL;
	h = (W_Height > map.height * LONGEUR_PIXEL) ? W_Height : map.height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("single triangle");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);
	// map.drop_water(10, 10, 40000);
	// map.drop_water(map.width / 2, map.height / 2, 400000);

	// La vague :
	for (int x = 0; x < map.width; x++)
		map.drop_water(x, 0, 1000);
	std::thread t([&map]{
		while (1)
		{
			// map.drop_water(map.width - map.width / 1.5, 100, 10);
			mtx.lock();
			if (q.size() <= RENDER_AHEAD)
			{
				map.apply_gravity();
				q.push(map);
			}
			if (q.size() > 0)
			{
				//std::cout << "display" << std::endl;
				glutPostRedisplay();
			}
			else
			{
				sleep(0);
			}
			mtx.unlock();
			// sleep(1 / FPS);
		}
	});
 	glutMainLoop();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
