#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define FPS 25.0
#define RENDER_AHEAD FPS * 2.0

extern std::queue<Map> q;
extern std::mutex mtx;
void idle();
