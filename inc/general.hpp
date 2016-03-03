#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define FPS 24.0
#define RENDER_AHEAD FPS / 10.0

extern std::queue<Map> q;
extern std::mutex mtx;
