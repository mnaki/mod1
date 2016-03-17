#include "Affichage.hpp"
#include "general.hpp"
#include <atomic>
#include <stdio.h>
#include <cmath>

bool	conf_rotate =		CONFIG_ROTATE;
bool	conf_skip_frames =	CONFIG_SKIP_FRAMES;
GLfloat	conf_zoom =			CONFIG_ZOOM;
bool	conf_pause =		CONFIG_PAUSE;
GLfloat target_rotate =		GLF_TARGET_ROTATE;
std::atomic<bool>           pour_water(true);
std::atomic<bool>           running(true);

void	keyboard(unsigned char ch, int x, int y)
{
	x = ch; y = ch;
	switch (ch)
	{
		case 27:             /* escape */
			running = !running;
			break;
		case 'w':
		case 'W':
			glTranslatef(0,-10, 0);
			glutPostRedisplay();
			break;
		case 's':
		case 'S':
			glTranslatef(0, 10, 0);
			glutPostRedisplay();
			break;
		case 'a':
		case 'A':
			glTranslatef(10, 0,0);
			glutPostRedisplay();
			break;
		case 'd':
		case 'D':
			glTranslatef(-10, 0,0);
			glutPostRedisplay();
			break;
		case 'Q':
		case 'q':
			conf_rotate = (conf_rotate) ? false : true;
			glutPostRedisplay();
			break;
		case 'X':
		case 'x':
			conf_skip_frames = !conf_skip_frames;
			std::cout << "conf_skip_frames = " << conf_skip_frames << std::endl;
			break;
		case '=':
			conf_zoom *= 0.9f;
			std::cout << "conf_zoom = " << conf_zoom << std::endl;
			break ;
		case '-':
			conf_zoom /= 0.9f;
			std::cout << "conf_zoom = " << conf_zoom << std::endl;
			break ;
		case 'r':
		case 'R':
			target_rotate += 45.0f * 2.0f;
			break;
		case 'P':
		case 'p':
			conf_pause = !conf_pause;
			break;
		case ' ':
			pour_water = !pour_water;
			break;
		case '1':
			glutReshapeFunc(reshape_vue_haute);
			glutDisplayFunc(display_vue_haute);
			reshape_vue_haute(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			glutPostRedisplay();
			break;
		case '2':
			glutReshapeFunc(reshape_iso);
			glutDisplayFunc(display_iso);
			reshape_iso(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			glutPostRedisplay();
			break;
		case '3':
			glutReshapeFunc(reshape);
			glutDisplayFunc(display);
			reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			glutPostRedisplay();
			break;
		default:
			break;
	}
}

void mouse_motion(int x, int y)
{
	static GLdouble oldx = x, oldy = y;
	if ((x - oldx) != 0)
	{
		glRotatef(1.0f, 0, 0.0f, (x - oldx));
	}
	oldx = x; oldy = y;
}
