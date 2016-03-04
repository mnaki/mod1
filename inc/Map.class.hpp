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
#include "MapPoint.class.hpp"

# define MAX_THREAD_COUNT (2.0)

# define YELLOW "\033[1;33m"
# define WHITE "\033[1;37m"
# define BLACK "\033[1;30m"
# define GREEN "\033[1;32m"
# define CYAN "\033[1;36m"
# define BLUE "\033[1;34m"

class Map
{
    public:

    	std::vector< std::vector<MapPoint> > data;
    	int const width;
    	int const height;
		std::string scenario;

    	Map(int width = 1, int height = 1, std::string scenario = "strilanka");
    	~Map() = default;
    	void		drop_water(int x, int y, int quantity);
		int			get_hauteur_max(void) const ;
    	void		apply_gravity(void);
    	void		elevate_rect(int x0, int y0, int x1, int y1, int value);
        void		draw_cone(int start_x, int start_y, int radius, int height, bool reverse = false);
	    std::string	to_string(void) const;
        int			resistance(int x, int y);
        int			resistance(MapPoint const & point);
};
