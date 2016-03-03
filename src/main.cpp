#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

void scenario_rain(Map & map)
{
	double size = (map.width + map.height) / 2.0;
	double maillage = 20.0;
	static int make_it_rain = FPS / maillage;
	for (size_t x = map.width - map.width / 2 - size / 2; x < map.width / 2 + size / 2; x += maillage) {
		for (size_t y = map.height - map.height / 2 - size / 2; y < map.height / 2 + size / 2; y += maillage) {
			map.drop_water(x, y, (make_it_rain % FPS == 0) * maillage * maillage);
		}
	}
	make_it_rain += 1;
}

void scenario_rain_middle(Map & map)
{
	static int make_it_rain = 1;
	for (int x = map.width / 2 - map.width / 10; x < map.width / 2 + map.width / 10; x++) {
		for (int y = map.height / 2 - map.height / 10; y < map.height / 2 + map.height / 10; y++) {
			map.drop_water(x, y, (make_it_rain % 40 == 0) * 4);
		}
	}
	make_it_rain += 1;
}

void scenario_srilanka(Map & map)
{
	for (int x = 0; x < map.width; x++) {
		map.drop_water(x, 0, 3);
	}
}

typedef enum map_e
{
	map_custom,
	map_volcano,
	map_beach,
	map_montagne
} map_e;

map_e current_map = map_montagne;

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map map(400, 400);
	int radius = map.width / 8;
	switch (current_map) {
		case map_beach: {
			for (int y = 0; y < map.height + radius * 2; y += radius * 3)
			{
				map.draw_cone(radius * 0, y - map.height / 2, radius, rand() % map.width/2 + map.width/1.5);
				map.draw_cone(radius * 2, y - map.height / 2, radius, rand() % map.width/2 + map.width/1.5);
				map.draw_cone(radius * 4, y - map.height / 2, radius, rand() % map.width/2 + map.width/1.5);
				map.draw_cone(radius * 6, y - map.height / 2, radius, rand() % map.width/2 + map.width/1.5);
				map.draw_cone(radius * 8, y - map.height / 2, radius, rand() % map.width/2 + map.width/1.5);
			}
		};
		break;

		case map_volcano: {
			map.draw_cone(map.width - radius * 4, map.height - radius * 4, map.height / 4, map.width - 0);
			map.draw_cone(map.width - radius * 4, map.height - radius * 4, map.height / 6, map.width / 1.3, true);
		}
		break;

		case map_montagne: {
			map.draw_cone(map.width - radius * 4, map.height - radius * 4, map.height / 8, map.width/1.4);
			map.draw_cone(map.width - radius * 5, map.height - radius * 4, map.height / 7, map.width/1.3);
			map.draw_cone(map.width - radius * 4, map.height - radius * 4, map.height / 6, map.width/1.2);
			map.draw_cone(map.width - radius * 3, map.height - radius * 4, map.height / 5, map.width/1.1);
		}
		break;
	}
	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map.width * LARGEUR_PIXEL) ? W_Width : map.width * LARGEUR_PIXEL;
	h = (W_Height > map.height * LONGEUR_PIXEL) ? W_Height : map.height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("Mod1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);


	std::thread t([&map]{
		while (1)
		{
			mtx.lock();
			if (q.size() <= RENDER_AHEAD)
			{
				scenario_rain(map);
				// scenario_rain_middle(map);
				// scenario_srilanka(map);
				map.apply_gravity();
				q.push(map);
			}
			if (q.size() > 0)
			{
				glutPostRedisplay();
			}
			mtx.unlock();
		}
	});
 	glutMainLoop();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
