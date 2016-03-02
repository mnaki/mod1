#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

void scenario_rain(Map & map)
{
	double size = (map.width + map.height) / 2.0;
	double maillage = 10.0;
	static int make_it_rain = 1;
	for (size_t x = map.width - map.width / 2 - size / 2; x < map.width / 2 + size / 2; x += maillage) {
		for (size_t y = map.height - map.height / 2 - size / 2; y < map.height / 2 + size / 2; y += maillage) {
			map.drop_water(x, y, (make_it_rain % 10 == 0) * maillage * maillage);
		}
	}
	make_it_rain += 1;
}

void scenario_rain_middle(Map & map)
{
	static int make_it_rain = 1;
	map.drop_water(map.width / 2, map.height / 2, (make_it_rain % 20 == 0) * 10000);
	make_it_rain += 1;
}

void scenario_srilanka(Map & map)
{
	for (int x = 0; x < map.width; x++) {
		map.drop_water(x, 0, 10);
	}
}

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(200, 200);
	int radius = map.width / 8;
	for (int y = 0; y < map.height - radius; y += radius * 2)
	{
		map.draw_cone(radius * 0, y + map.width / 2, radius, 500);
		map.draw_cone(radius * 2, y + map.width / 2, radius, 500);
		map.draw_cone(radius * 4, y + map.width / 2, radius, 500);
		map.draw_cone(radius * 6, y + map.width / 2, radius, 500);
		map.draw_cone(radius * 8, y + map.width / 2, radius, 500);
	}
	// map.elevate_rect(0, 0, map.width, map.height, 100);
	map.viscosity = 0;

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map.width * LARGEUR_PIXEL) ? W_Width : map.width * LARGEUR_PIXEL;
	h = (W_Height > map.height * LONGEUR_PIXEL) ? W_Height : map.height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("Mod1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);

  	q.push(map);
	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() <= RENDER_AHEAD)
			{
				// scenario_rain(map);
				// scenario_srilanka(map);
				scenario_rain_middle(map);
				map.apply_gravity();
				q.push(map);
			}
			if (q.size() > 0)
			{
				glutPostRedisplay();
			}
			mtx.unlock();
			usleep(10000);
		}
	});
 	glutMainLoop();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
