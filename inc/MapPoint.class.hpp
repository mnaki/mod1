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

struct MapPoint
{
	std::atomic_int terrain_height;
	std::atomic_int water_level;
	MapPoint();
	MapPoint(const MapPoint & rhs);
	~MapPoint();
	MapPoint & operator=(const MapPoint & rhs);
};
