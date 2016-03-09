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

#define CONFIG_max_points	40		// int			// Nombre de poygones par côté
#define CONFIG_taille_carre	0.1		// double		// Taille d'un poygone
#define CONFIG_FPS 			FPS		// int

#define CONFIG_ROTATE		false	// bool			// Terrain qui tourne ou non
#define CONFIG_SKIP_FRAMES	false	// bool
#define CONFIG_ZOOM			0.003f	// GLfloat
#define CONFIG_PAUSE		0		// int

#define GLF_ROTATE			0.0f // GLfloat
#define GLF_TARGET_ROTATE	-45.0f //GLfloat

//void	reshape(int w, int h);

void	display_vue_haute(void);
void	reshape_vue_haute(int w, int h);

void	display_iso(void);
void	reshape_iso(int w, int h);

void	display(void);
void	reshape(int w, int h);

void	keyboard(unsigned char ch, int x, int y);
void	mouse_motion(int x, int y);
