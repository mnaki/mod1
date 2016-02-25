#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define FPS 15
#define RENDER_AHEAD 3

extern std::queue<Map> q;
extern std::mutex mtx;
