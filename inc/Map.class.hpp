#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <atomic>
#include <mutex>
#include <thread>
#include <list>
#include "MapPoint.class.hpp"

# define YELLOW "\033[1;33m"
# define WHITE "\033[1;37m"
# define BLACK "\033[1;30m"
# define GREEN "\033[1;32m"
# define CYAN "\033[1;36m"
# define BLUE "\033[1;34m"

struct RainDrop
{
    int x = 0;
    int y = 0;
    int altitude = 0;
    float mass = 0.0f;
};

class Map
{
    public:

    	std::vector< std::vector<MapPoint> > data;
        std::vector< RainDrop >              rain_drops;
    	int width;
    	int height;
		int conf_marge_bocal = 1;
		int scenario;

    	Map(int width = 1, int height = 1, int scenario = 0);
    	~Map() = default;
        void        update_rain(void);
        void        drop_rain(int x, int y, float mass);
    	void		drop_water(int x, int y, float quantity);
    	void		set_water(int x, int y, float value);
		int			get_hauteur_max(void) const ;
    	void		apply_gravity(void);
		void		ecoulement_unitaire(int x, int y, int i, int j);
    	void		elevate_rect(int x0, int y0, int x1, int y1, float value);
        void		draw_cone(int start_x, int start_y, int radius, int height, bool reverse = false);
	    std::string	to_string(void) const;
        float		resistance(int x, int y);
        float		resistance(MapPoint const & point);
};
