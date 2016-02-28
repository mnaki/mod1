#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

void scenario_rain(Map & map)
{
	static int make_it_rain = 1;
	for (size_t x = 0; x < map.width; x += 18) {
		for (size_t y = 0; y < map.height; y += 18) {
			map.drop_water(x, y, (make_it_rain % 18 == 0) * 9);
		}
	}
	make_it_rain += 1;
	map.apply_gravity();
}

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(101, 101);
	// map.elevate_rect(map.width / 3.5, map.height / 3.5, map.width - map.width / 3.5, map.height - map.height / 3.5, 20);
	// map.elevate_rect(map.width / 3, map.height / 3, map.width - map.width / 3, map.height - map.height / 3, -10);
	// map.elevate_rect(map.width / 2.1, map.height / 2.1, map.width - map.width / 2.1, map.height - map.height / 2.1, 80);
	// map.elevate_rect(0, 0, map.width, map.height, 10);
	map.draw_cone(map.width / 2, map.height / 2, (map.width + map.height) / 8, (map.width + map.height) / 4);
	map.viscosity = 0.0;

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map.width * LARGEUR_PIXEL) ? W_Width : map.width * LARGEUR_PIXEL;
	h = (W_Height > map.height * LONGEUR_PIXEL) ? W_Height : map.height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("single triangle");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);

	// map.drop_water(map.width / 4, map.height / 4, 1000);

	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() <= RENDER_AHEAD)
			{
				scenario_rain(map);
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
