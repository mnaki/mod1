#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define FPS 25.0
#define RENDER_AHEAD FPS * 2.0

extern std::queue<Map>		q;
extern std::mutex			mtx;
extern bool					conf_skip_frames;
extern bool					conf_pause;

extern bool					conf_rotate;
extern GLfloat				conf_zoom;
extern GLfloat				rotate;
extern GLfloat				target_rotate;

extern std::atomic<bool>	pour_water;
extern std::atomic<bool>	running;

void idle();
