#include "Affichage.hpp"
#include "general.hpp"
#include "parser.hpp"
#include <sched.h>
#include <time.h>
#include <atomic>

std::queue<Map> q;
std::mutex mtx;
extern std::atomic<bool> pour_water;
extern std::atomic<bool> running;

void flood_uniform(Map * map)
{
	static float level = 0.0f;
	for (int x = 0; x < map->width ; x += 1) {
		for (int y = 0; y < map->height ; y += 1) {
			if (map->resistance(x, y) < level)
				map->drop_water(x, y, 1.0f);
		}
	}
	level += 0.1f;
}

void scenario_rain(Map * map)
{
	for (int x = 0; x < map->width ; x += 1) {
		for (int y = 0; y < map->height ; y += 1) {
			if ((rand() % 2 + 1) > 1)
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
		map->drop_water(x, map->height - 2, level_water);
	}
}

void scenario_riviere(Map * map)
{
	map->drop_water(30, map->height / 4.01, 20);
}

void idle()
{
	struct timespec t{ 0, 10000000 };
	nanosleep(&t, NULL);
	sched_yield();
}

void glutTimer(int te)
{
	(void)te;
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, glutTimer, 1);
}

int	main(int ac, char **av)
{
	// initiation du programe
	glutInit(&ac, av);
	srand(time(NULL));

	Map *map;
	if (ac != 2)
	{
		std::cout << "usage : mod1 fichier.mod1" << std::endl;
		return 0;
	}

	map = init_from_fichier(av[1]);

	// creation de la fenetre en fonction de la map
	int w = 0, h = 0;
	w = (W_Width > map->width * LARGEUR_PIXEL) ? W_Width : map->width * LARGEUR_PIXEL;
	h = (W_Height > map->height * LONGEUR_PIXEL) ? W_Height : map->height	* LONGEUR_PIXEL;
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(w, h);
	glutCreateWindow("Mod1");
	glutReshapeFunc(reshape_iso);
	glutDisplayFunc(display_iso);
	glutMotionFunc(mouse_motion);
	glutTimerFunc(1000 / FPS, glutTimer, 1);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	std::thread t([&map]{
		try
		{
			while (running)
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
						case 4:
						flood_uniform(map);
						break;
						default:
						scenario_rain(map);
					}
					map->apply_gravity();
					idle();

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
					mtx.lock();
					q.push(*map);
				}
				if (q.size() > 0)
				{
					mtx.unlock();
					idle();
				}
				else
				{
					mtx.unlock();
				}
				idle();
				mtx.unlock();
			}
			exit(0);
		}
		catch (std::exception & e)
		{
			std::cout << e.what() << std::endl;
		}
	});
	glutMainLoop();
	t.join();
	(void)ac;
	(void)av;
	return 0;
}
