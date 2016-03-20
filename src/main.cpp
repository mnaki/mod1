#include "Affichage.hpp"
#include "general.hpp"
#include "parser.hpp"
#include <sched.h>
#include <time.h>
#include <atomic>

std::queue<Map>          q;
std::mutex               mtx;
extern std::atomic<bool> pour_water;
extern std::atomic<bool> running;
int                      CONF_NUM_CORES;

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

void flood_from_borders(Map * map)
{
    for (int y = 0; y < map->height ; y += 1)
    {
        map->set_water(0, y, map->get_hauteur_max());
    }
    for (int y = 0; y < map->height ; y += 1)
    {
        map->set_water(map->width - 1, y, map->get_hauteur_max());
    }

    for (int x = 0; x < map->width ; x += 1)
    {
        map->set_water(x, 0, map->get_hauteur_max());
    }
    for (int x = 0; x < map->width ; x += 1)
    {
        map->set_water(x, map->height - 1, map->get_hauteur_max());
    }
}

void scenario_rain(Map * map)
{
	for (int x = 0; x < map->width ; x += 1) {
		for (int y = 0; y < map->height ; y += 1) {
			if (rand() % 10000 == 0)
				map->drop_rain(x, y, 0.20f);
		}
	}
}

void scenario_snow(Map * map)
{
	for (int x = 0; x < map->width ; x += 1) {
		for (int y = 0; y < map->height ; y += 1) {
			if (rand() % 1000000 == 0)
				map->drop_snow(x, y, 0.20f);
		}
	}
}

void scenario_rain_middle(Map * map)
{
    int size = 10;
    for (int x = 80 - size; x < 80 + size; x++)
    for (int y = 60 - size; y < 60 + size; y++)
        map->drop_rain(x, y, 0.2f * (rand() % 100 == 0));
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
    static int make_it_rain = 1;
    if (make_it_rain % 10 == 0)
        map->drop_rain(40, map->height / 3, 1.0f);
    make_it_rain++;
}

void idle()
{
	sched_yield();
    sleep(0);
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

	CONF_NUM_CORES = std::thread::hardware_concurrency() - 1;
    if (CONF_NUM_CORES <= 0)
        CONF_NUM_CORES = 1;
	std::cout << "CONF_NUM_CORES = " << CONF_NUM_CORES << std::endl;

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
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMotionFunc(mouse_motion);
	glutTimerFunc(1000 / FPS, glutTimer, 1);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	std::thread t([&map]{
		try
		{
			while (running)
			{
				if (q.size() < RENDER_AHEAD)
				{
					if (pour_water && !vidange)
                    {
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
							case 5:
	                            flood_from_borders(map);
	                            break;
							case 6:
	                            scenario_snow(map);
	                            break;
							default:
	                            scenario_rain(map);
	                            break;
						}
                    }
					map->apply_gravity();

                    if (vidange)
                    {
                        static int deja_vidange = false;
                        if (deja_vidange == false)
                        {
                            auto ref = map->get_hauteur_max();
                            for (int x = map->width * 0.2; x < map->width * 0.8; x++)
                            for (int y = map->width * 0.2; y < map->height * 0.8; y++)
                            {
                                map->data[x][y].terrain_height = -20;
                            }
                            map->draw_cone(map->width / 2, map->height / 2, map->width * 0.2, ref * 150, true);
                            std::cout << "vidange" << std::endl;
                        }
                        deja_vidange = true;
                    }

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

					q.push(*map);
				}
				if (q.size() > 2)
				{
                    usleep(1000000/FPS);
                    idle();
				}
				idle();
			}
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
