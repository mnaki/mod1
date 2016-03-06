#include "Affichage.hpp"
#include "general.hpp"

std::queue<Map> q;
std::mutex mtx;

void scenario_rain(Map * map)
{
	double size = (map->width + map->height) / 2.0;
	double maillage = 10.0;
	for (size_t x = map->width - map->width / 2 - size / 2; x < map->width / 2 + size / 2; x += maillage) {
		for (size_t y = map->height - map->height / 2 - size / 2; y < map->height / 2 + size / 2; y += maillage) {
			map->drop_water(x, y, 1);
		}
	}
}

void scenario_rain_middle(Map * map)
{
	for (int x = map->width / 2 - map->width / 70; x < map->width / 2 + map->width / 70; x++) {
		for (int y = map->height / 2 - map->height / 70; y < map->height / 2 + map->height / 70; y++) {
			map->drop_water(x, y, 2);
		}
	}
}

void scenario_srilanka(Map * map)
{
	// static int level_water = -1;
	// if (level_water == -1) {level_water = map->get_hauteur_max();}
	for (int x = 0; x < map->width; x++) {
		map->drop_water(x, map->height - 2, 0.1);
	}
}

void scenario_riviere(Map * map)
{
	map->drop_water(30, map->height / 4.01, 10);
}

void idle()
{
	usleep(1000);
}

typedef enum map_e
{
	map_volcano,
	map_beach,
	map_montagne,
	map_riviere
} map_e;

map_e current_map = map_beach;

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	// creation de la map
	Map *map = new Map(200, 200);
	int radius = map->width / 10.0;
	switch (current_map) {
		case map_beach: {
			for (int y = 0; y < map->height + radius * 2; y += radius * 1.5)
			{
				map->draw_cone(radius * 0, y - map->height / 2,  rand() % radius * 1.5 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 2, y - map->height / 2,  rand() % radius * 1.5 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 4, y - map->height / 2,  rand() % radius * 1.5 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 6, y - map->height / 2,  rand() % radius * 1.5 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 8, y - map->height / 2,  rand() % radius * 1.5 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
			}
		};
		break;

		case map_riviere: {
			int y = 0;
			for (int i = map->width - 1; i >= 0; i--)
			{
				map->elevate_rect(i, 0, i+1, map->width - 1, y * 0.25);
				y++;
			}
			map->draw_cone(0, 0, 50, 100);

			for (int y = 0; y < map->height + radius * 2; y += radius * 1.5)
			{
				map->draw_cone(radius * 0, y - map->height / 2,  rand() % radius * 1.4 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 2, y - map->height / 2,  rand() % radius * 1.4 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 4, y - map->height / 2,  rand() % radius * 1.4 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 6, y - map->height / 2,  rand() % radius * 1.4 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
				map->draw_cone(radius * 8, y - map->height / 2,  rand() % radius * 1.4 + radius * 1, rand() % map->width/1.6 + map->width/1.5);
			}
		};
		break;

		case map_volcano: {
			map->draw_cone(map->width - radius * 5, map->height - radius * 5, map->height / 6, map->width / 1.1);
			map->draw_cone(map->width - radius * 5, map->height - radius * 5, map->height / 8, map->width / 1.3, true);
		}
		break;

		case map_montagne: {
			map->draw_cone(map->width - radius * 4, map->height - radius * 4, map->height / 8, map->width/1.4);
			map->draw_cone(map->width - radius * 5, map->height - radius * 4, map->height / 7, map->width/1.3);
			map->draw_cone(map->width - radius * 4, map->height - radius * 4, map->height / 6, map->width/1.2);
			map->draw_cone(map->width - radius * 3, map->height - radius * 4, map->height / 5, map->width/1.1);
		}
		break;
	}
	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map->width * LARGEUR_PIXEL) ? W_Width : map->width * LARGEUR_PIXEL;
	h = (W_Height > map->height * LONGEUR_PIXEL) ? W_Height : map->height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("Mod1");
	glutDisplayFunc(display);
//	glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);
  	glutIdleFunc(idle);

	std::thread t([&map]{
		while (1)
		{
			try
			{
				mtx.lock();
				if (q.size() <= RENDER_AHEAD)
				{
					mtx.unlock();

					if (current_map == map_montagne)
						scenario_rain(map);
					if (current_map == map_volcano)
						scenario_rain_middle(map);
					if (current_map == map_beach)
						scenario_srilanka(map);
					if (current_map == map_riviere)
						scenario_riviere(map);
					map->apply_gravity();
					mtx.lock();

				  	// bordures nulles
				  	for (int x = 0; x < map->width; x++)
				  	{
				  		map->data[x][0].terrain_height = 0;
				  		map->data[x][map->height-1].terrain_height = 0;
				  		map->data[x][0].water_level = 0;
				  		map->data[x][map->height-1].water_level = 0;
				  	}
				  	for (int y = 0; y < map->height; y++)
				  	{
				  		map->data[0][y].terrain_height = 0;
				  		map->data[map->width-1][y].terrain_height = 0;
				  		map->data[0][y].water_level = 0;
				  		map->data[map->width-1][y].water_level = 0;
				  	}
				  	//
					q.push(*map);
				}
				else if (q.size() > 0)
					glutPostRedisplay();
				mtx.unlock();
			}
			catch (std::exception & e)
			{
				std::cout << e.what() << std::endl;
			}
		}
	});
	t.detach();
 	glutMainLoop();
	(void)ac;
	(void)av;
	return 0;
}
