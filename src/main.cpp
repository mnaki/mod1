#include "Affichage.hpp"
#include "general.hpp"
#include "parser.hpp"

std::queue<Map> q;
std::mutex mtx;
extern bool pour_water;
bool running = true;

void scenario_rain(Map * map)
{
	for (size_t x = 0; x < map->width ; x += 2) {
		for (size_t y = 0; y < map->height ; y += 2) {
			map->drop_water(x, y, 0.01f);
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
	static int level_water = -1;
	if (level_water == -1) {level_water = map->get_hauteur_max();}
	for (int x = 0; x < map->width; x++) {
		map->set_water(x, map->height - 2, level_water);
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

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);

	Map *map;
	if (ac != 2)
	{
		std::cout << "usage : mod1 fichier.map" << std::endl;
		return 0;
	}

	map = init_from_fichier(av[1]);

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map->width * LARGEUR_PIXEL) ? W_Width : map->width * LARGEUR_PIXEL;
	h = (W_Height > map->height * LONGEUR_PIXEL) ? W_Height : map->height	* LONGEUR_PIXEL;
	glutInitWindowSize(w, h);
	glutCreateWindow("Mod1");
	glutDisplayFunc(display);
  	glutKeyboardFunc(keyboard);
  	glutIdleFunc(idle);

	std::thread t([&map]{
		while (running)
		{
			try
			{
				mtx.lock();
				if (q.size() <= RENDER_AHEAD)
				{
					mtx.unlock();
					if (pour_water)
						switch(map->scenario)
						{
							case 0:
								scenario_rain(map);
								break;
							case 1:
								scenario_rain_middle(map);
								break;
							case 2:
								scenario_srilanka(map);
								break;
							case 3:
								scenario_riviere(map);
								break;
							default:
								scenario_rain(map);
						}
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
