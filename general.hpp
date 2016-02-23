#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define RENDER_AHEAD 30
#define FPS 30

extern std::queue<Map> q;
extern std::mutex mtx;
