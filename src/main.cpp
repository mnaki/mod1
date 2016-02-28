#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(20, 20);
	// map.elevate_rect(map.width / 3.5, map.height / 3.5, map.width - map.width / 3.5, map.height - map.height / 3.5, 20);
	// map.elevate_rect(map.width / 3, map.height / 3, map.width - map.width / 3, map.height - map.height / 3, -10);
	// map.elevate_rect(map.width / 2.1, map.height / 2.1, map.width - map.width / 2.1, map.height - map.height / 2.1, 80);
	// map.elevate_rect(0, 0, map.width, map.height, 10);
	map.draw_cone(map.width / 2, map.height / 2, (map.width + map.height) / 8, 20);

	map.viscosity = 0.7;
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

	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() <= RENDER_AHEAD)
			{
				// for (size_t x = 0; x < map.width; x++) {
				// 	for (size_t y = 0; y < map.height; y++) {
				// 		map.drop_water(x, y, 1);
				// 	}
				// }

				// for (size_t y = 0; y < map.height; y++) {
				// 	map.drop_water(0, y, 1);
				// }
				map.drop_water(map.width / 2 + 2, map.height / 2, 1);
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
