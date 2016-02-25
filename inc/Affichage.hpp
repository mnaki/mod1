#pragma once

#include "MapPoint.class.hpp"
#include "Map.class.hpp"

#ifdef __LINUX__
# include <GL/glut.h>
#endif

#ifdef __WIN__
# include <GL/glut.h>
# include <GL/glu.h>
# include <GL/gl.h>
# include <windows.h>
#endif

#ifdef __MAC__
# include <GLUT/glut.h>
#endif

#define MAX_LVL_WATER	100
#define MAX_LVL_TERRAIN	85
#define W_Width			1200
#define W_Height		800
#define LARGEUR_PIXEL	2
#define LONGEUR_PIXEL	2

void	reshape(int w, int h);
void	display(void);
