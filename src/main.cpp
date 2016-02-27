#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(400, 400);
	map.elevate_rect(map.width / 5, map.height / 5, map.width - map.width / 5, map.height - map.height / 5, 5);
	map.elevate_rect(map.width / 3, map.height / 3, map.width - map.width / 3, map.height - map.height / 3, -3);
	map.elevate_rect(map.width / 2.1, map.height / 2.1, map.width - map.width / 2.1, map.height - map.height / 2.1, 100);
	map.elevate_rect(0, 0, map.width, map.height, 0);

	map.viscosity = 0.0;
	std::cout << "started" << std::endl;

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map.width * LARGEUR_PIXEL) ? W_Width : map.width * LARGEUR_PIXEL;
	h = (W_Height > map.height * LONGEUR_PIXEL) ? W_Height : map.height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("single triangle");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);

	// La vague :
	for (int x = 0; x < map.width; x++)
	{
		map.drop_water(x, 0, 10000);
	}

	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() <= RENDER_AHEAD)
			{
				map.apply_gravity();
				q.push(map);
			}
			if (q.size() > 0)
			{
				glutPostRedisplay();
			}
			mtx.unlock();
			sleep(0);
		}
	});
 	glutMainLoop();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
