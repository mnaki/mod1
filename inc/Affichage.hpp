#pragma once

#include "MapPoint.class.hpp"
#include "Map.class.hpp"

#ifdef __LINUX__
# include <GL/glut.h>
#else
# ifdef __WIN__
#  include <GL/glut.h>
#  include <GL/glu.h>
#  include <GL/gl.h>
#  include <windows.h>
# else
#  include <GLUT/glut.h>
# endif
#endif

#define MAX_LVL_WATER	100
#define MAX_LVL_TERRAIN	85
#define W_Width			1800
#define W_Height		1600
#define LARGEUR_PIXEL	1
#define LONGEUR_PIXEL	1

void	reshape(int w, int h);
void	display(void);
void	keyboard(unsigned char ch, int x, int y);
