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
#include <cstring>

struct MapPoint
{
	std::atomic<long int> terrain_height;
	std::atomic<long int> water_level;
	MapPoint();
	MapPoint(const MapPoint & rhs);
	~MapPoint();
	MapPoint & operator=(const MapPoint & rhs);
};
