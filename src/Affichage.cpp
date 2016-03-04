#include "Affichage.hpp"
#include "general.hpp"

#include <stdio.h>

bool	conf_rotate =		CONFIG_ROTATE;
bool	conf_skip_frames =	CONFIG_SKIP_FRAMES;
GLfloat	conf_zoom =			CONFIG_ZOOM;
bool	conf_pause =		CONFIG_PAUSE;
GLfloat rotate =			GLF_ROTATE;
GLfloat target_rotate =		GLF_TARGET_ROTATE;

#define WIDTH 640                       // Largeur de la fenêtre
#define HEIGHT 480                      // Hauteur de la fenêtre
#define MAX_POINTS 80                   // Nombre MAX de poly
#define MIN_POINTS 15                   // Nombre MIN de poly

float xpos = -0.5f, ypos=-0.5f, zpos = -0.0f;

/* ARGSUSED1 */
void	keyboard(unsigned char ch, int x, int y)
{
	x = ch; y = ch;
	switch (ch)
	{
		case 27:             /* escape */
			exit(0);
			break;
		case 'w':
		case 'W':
			ypos+=0.1f;
			glutPostRedisplay();
			break;
		case 's':
		case 'S':
			ypos-=0.1f;
			glutPostRedisplay();
			break;
		case 'a':
		case 'A':
			xpos-=0.1f;
			glutPostRedisplay();
			break;
		case 'd':
		case 'D':
			xpos+=0.1f;
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
			target_rotate += 45.0f * 2;
			break;
		case 'P':
		case 'p':
			conf_pause = !conf_pause;
		default:
				  break;
	}
}

void set_color(Map const & cmap, int x, int y)
{
	GLfloat deepness = (cmap.data[x][y].water_level + cmap.data[x][y].terrain_height) / (cmap.data[x][y].terrain_height + 40.0f);
	if (cmap.data[x][y].water_level > 0)
	{
		glColor4f( 0, 0.4f / deepness, 0.9f / deepness, 0.75f );
	}
	else
	{
		glColor4f( 1.0f - 0.5f * deepness, 1.0f - 0.25f * deepness, 1.0f - 0.75f * deepness, 1.0f );
	}
}
void	display(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glFrustum(-1.0, 1.0, -1.0, 1.0, 5, 100);
	// glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);

	mtx.lock();
	if (q.size() <= 0)
	{
		mtx.unlock();
		std::cout << "no frame" << std::endl;
		return ;
	}

	Map cmap = q.back();

	if (!conf_pause)
	{
		if (conf_skip_frames)
		{
			while (q.size() > 0)
			{
				q.pop();
			}
		}
		else
		{
			q.pop();
		}
	}

	mtx.unlock();


	if (conf_rotate)
	{
		rotate -= 45.0f/8.0f;
	}

	rotate += (target_rotate + - rotate) / 8.0f;
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glLoadIdentity();
	glTranslatef(xpos, ypos, zpos);

	glScalef(conf_zoom, conf_zoom, conf_zoom);
	glRotatef(45.0f, -1.0f, 0.0f, 0.0f);
	glRotatef(rotate, 0.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	for (GLfloat x = 0; x < cmap.width - 1; x++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat y = 0; y < cmap.height - 1; y++)
		{
			set_color(cmap, x, y+1);
			glVertex3f( x , y+1 , cmap.data[x][y+1].terrain_height + cmap.data[x][y+1].water_level );

			set_color(cmap, x, y);
			glVertex3f( x , y , cmap.data[x][y].terrain_height + cmap.data[x][y].water_level );

			set_color(cmap, x+1, y+1);
			glVertex3f( x+1 , y+1 , cmap.data[x+1][y+1].terrain_height + cmap.data[x+1][y+1].water_level );

			set_color(cmap, x+1, y);
			glVertex3f( x+1 , y , cmap.data[x+1][y].terrain_height + cmap.data[x+1][y].water_level );
		}
		glEnd();
	}

	glFlush();

	if (!conf_skip_frames)
	{
		usleep(1000000 / CONFIG_FPS);
	}
}
