#pragma once
#include <queue>
#include <mutex>

#include "Map.class.hpp"

#define FPS (60.0)
#define RENDER_AHEAD 3

#define WIDTH 200
#define HEIGHT 200

extern std::queue<Map>		q;
extern std::mutex			mtx;
extern bool					conf_skip_frames;
extern bool					conf_pause;

extern bool					conf_rotate;
extern float				conf_zoom;
extern float				rotate;
extern float				target_rotate;

extern std::atomic<bool>	pour_water;
extern std::atomic<bool>	running;
extern int                  CONF_NUM_CORES;
extern std::atomic<bool>    vidange;

void idle();
