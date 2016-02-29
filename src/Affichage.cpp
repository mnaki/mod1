#include "Affichage.hpp"
#include "general.hpp"

#include <stdio.h>

int      CONFIG_max_points = 40;        // Nombre de poygones par côté
double   CONFIG_taille_carre = 0.1;    // Taille d'un poygone
bool     CONFIG_rotate = false;         // Terrain qui tourne ou non
bool     CONFIG_SKIP_FRAMES = false;
GLfloat  CONFIG_ZOOM = 0.1f;

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

GLfloat rotate = 45.0f;
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
	if (CONFIG_SKIP_FRAMES)
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

			glBegin(GL_QUADS);
	for (int x = cmap.width - 1; x >= 0; x--)
	{
		for (int y = cmap.height - 1; y >= 0; y--)
		{
			GLfloat deepness;
			deepness = (cmap.data[x][y].water_level + cmap.data[x][y].terrain_height) / (cmap.data[x][y].terrain_height + 100.0f);
			if (cmap.data[x][y].water_level > 0)
			{
				glColor4f(
					0,
					0.4f / deepness,
					0.9f / deepness,
					0.9f
					);
			}
			else
			{
				glColor4f(
					1.0f - 0.5f * deepness,
					1.0f - 0.25f * deepness,
					1.0f - 0.75f * deepness,
					1.0f
					);
			}

            // glTexCoord3f( x + cmap.width / 10.0f, y + cmap.height / 10.0f, 0.0f + cmap.data[x][y].terrain_height / 100.0f  );
	        


	        glVertex3f( (-1.0f + x) / 10.0f , ( 1.0f + y) / 10.0f , 0.0f + cmap.data[x][y].terrain_height / 100.0f );              // Top Left
	        if (x < cmap.width - 1)
		        glVertex3f( ( 1.0f + x) / 10.0f , ( 1.0f + y) / 10.0f , 0.0f + cmap.data[x+1][y].terrain_height / 100.0f );              // Top Right
		    else
		        glVertex3f( ( 1.0f + x) / 10.0f , ( 1.0f + y) / 10.0f , 0.0f + 0 / 100.0f );              // Top Right
		    if (x < cmap.width - 1 && y < cmap.height - 1)
		        glVertex3f( ( 1.0f + x) / 10.0f , (-1.0f + y) / 10.0f , 0.0f + cmap.data[x+1][y+1].terrain_height / 100.0f );              // Bottom Right
		    else
		        glVertex3f( ( 1.0f + x) / 10.0f , (-1.0f + y) / 10.0f , 0.0f + 0 / 100.0f );              // Bottom Right
		    if (y < cmap.height - 1)
		        glVertex3f( (-1.0f + x) / 10.0f , (-1.0f + y) / 10.0f , 0.0f + cmap.data[x][y+1].terrain_height / 100.0f );              // Bottom Left
		    else
		        glVertex3f( (-1.0f + x) / 10.0f , (-1.0f + y) / 10.0f , 0.0f + 0 / 100.0f );              // Bottom Left

	    }
	}
	        glEnd();






	glFlush();


	usleep(1000000 / FPS);
}
