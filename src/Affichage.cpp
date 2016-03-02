#include "Affichage.hpp"
#include "general.hpp"

#include <stdio.h>

int      CONFIG_max_points = 40;        // Nombre de poygones par côté
double   CONFIG_taille_carre = 0.1;    // Taille d'un poygone
bool     CONFIG_rotate = false;         // Terrain qui tourne ou non
bool     CONFIG_SKIP_FRAMES = false;
GLfloat  CONFIG_ZOOM = 0.003f;

#define WIDTH 640                       // Largeur de la fenêtre
#define HEIGHT 480                      // Hauteur de la fenêtre
#define MAX_POINTS 80                   // Nombre MAX de poly
#define MIN_POINTS 15                   // Nombre MIN de poly

float xpos = -0.5f, ypos=-0.5f, zpos = -0.0f;

/* ARGSUSED1 */
void	keyboard(unsigned char ch, int x, int y)
{
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
		CONFIG_rotate = (CONFIG_rotate) ? false : true;
		glutPostRedisplay();
		break;
		case 'X':
		case 'x': {
			CONFIG_SKIP_FRAMES = !CONFIG_SKIP_FRAMES;
			std::cout << "CONFIG_SKIP_FRAMES = " << CONFIG_SKIP_FRAMES << std::endl;
		}
		break;
		case '=': {
			CONFIG_ZOOM *= 0.9f;
			std::cout << "CONFIG_ZOOM = " << CONFIG_ZOOM << std::endl;
		}
		break ;
		case '-': {
			CONFIG_ZOOM /= 0.9f;
			std::cout << "CONFIG_ZOOM = " << CONFIG_ZOOM << std::endl;
		}
		break ;
		default:
		break;
	}
}


void	reshape(int w, int h)
{

}

GLfloat rotate = 215.0f;

void set_color(Map const & cmap, int x, int y)
{
	GLfloat deepness = (cmap.data[x][y].water_level + cmap.data[x][y].terrain_height) / (cmap.data[x][y].terrain_height + 30.0f);
	if (cmap.data[x][y].water_level > 0)
	{
		glColor4f( 0, 0.4f / deepness, 0.9f / deepness, 0.9f );
	}
	else
	{
		glColor4f( 1.0f - 0.5f * deepness, 1.0f - 0.25f * deepness, 1.0f - 0.75f * deepness, 1.0f );
	}
}
void	display(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	// glClearDepth(40.0f);									// Depth Buffer Setup
	// glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDisable (GL_DEPTH_TEST);
	glDepthFunc(GL_NEVER);
	// glDepthFunc(GL_LEQUAL);
	// glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	// glLoadIdentity();


	mtx.lock();
	if (q.size() <= 0)
	{
		mtx.unlock();
		return ;
	}

	Map cmap = q.back();
	q.pop();
	if (CONFIG_SKIP_FRAMES)
	{
		while (q.size() > 0)
		{
			q.pop();
		}
	}
	mtx.unlock();


	if (CONFIG_rotate)
		rotate += 1.1f;
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glLoadIdentity();
	glTranslatef(xpos, ypos, zpos);

	glRotatef(-80.0f,0.0f,0.0f,0.0f);
	glRotatef(0.0f,0.0f,0.0f,1.0f);
	glRotatef(-30.0f,1.0f,0.0f,0.0f);
	glRotatef(10.0f + rotate,0.0f,0.0f,1.0f);
	glScalef(CONFIG_ZOOM, CONFIG_ZOOM, CONFIG_ZOOM);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	for (GLfloat x = cmap.width - 2; x >= 0; x--)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat y = cmap.height - 2; y >= 0; y--)
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

	if (!CONFIG_SKIP_FRAMES)
	{
		usleep(1000000 / FPS);
	}
}
