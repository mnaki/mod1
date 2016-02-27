#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define FPS 60
#define RENDER_AHEAD FPS / 10

extern std::queue<Map> q;
extern std::mutex mtx;
