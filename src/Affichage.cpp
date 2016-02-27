#include "Affichage.hpp"
#include "general.hpp"

#include <stdio.h>

int      CONFIG_max_points = 40;        // Nombre de poygones par côté
double   CONFIG_taille_carre = 1.0;    // Taille d'un poygone
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
			CONFIG_ZOOM += 0.1f;
			std::cout << "CONFIG_ZOOM = " << CONFIG_ZOOM << std::endl;
		}
			break ;
		case '-': {
			CONFIG_ZOOM -= 0.1f;
			std::cout << "CONFIG_ZOOM = " << CONFIG_ZOOM << std::endl;
		}
			break ;
		default:
			break;
	}
}


void	reshape(int w, int h)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
//	glClearDepth(40.0f);									// Depth Buffer Setup
//	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


	glMatrixMode(GL_PROJECTION);
	gluPerspective(	/* field of view in degree */ 20.0,
					/* aspect ratio */ 1.0,
					/* Z near */ 00.0, /* Z far */ 100.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(	0.0, 8.0, -5.0,	/* eye is at (0,8,60) */
				0.0, 0.0, 0.0,	/* center is at (0,8,0) */
				0.0, 1.0, 0.);	/* up is in postivie Y direction */

/*	glViewport(0, 0, (GLsizei)1200, (GLsizei)800);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 90.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1.0, 1.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);*/
}

GLfloat rotate = 36.0f;
void	display(void)
{
    mtx.lock();
    if (q.size() <= 0)
    {
    	// sleep(0);
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


	if (CONFIG_rotate) rotate += 1.1f;
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	int i,j;
	double ajout = CONFIG_taille_carre,posi,posj;

	glLoadIdentity();
	glTranslatef(xpos, ypos, zpos);

	glRotatef(-80.0f,0.0f,0.0f,0.0f);
	glRotatef(0.0f,0.0f,0.0f,1.0f);
	glRotatef(-30.0f,1.0f,0.0f,0.0f);
	glRotatef(10.0f + rotate,0.0f,0.0f,1.0f);
	glScalef(CONFIG_ZOOM, CONFIG_ZOOM, CONFIG_ZOOM);
	std::cout << "rotate = " << rotate << std::endl;


	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

					// for(i=0,posi=0 ;i<cmap.width-1;i++, posi += ajout)
					// {
				    //     glBegin(GL_TRIANGLE_STRIP);
					// 	for(j=0, posj=0;j<cmap.height-1;j++, posj += ajout)
					// 	{
					// 		glColor4f(0.0f,cmap.data[i][j].terrain_height+0.1f / (cmap.data[i][j].water_level+0.01f),0.0f, 1.0f);
				    //         // premier triangle
					// 		glTexCoord2f( (float)j / cmap.height, (float)i/cmap.width);
					// 		glVertex3d(posi,posj,cmap.data[i][j].terrain_height/100.0);
					//
					// 		glTexCoord2f( ((float)j+1)/cmap.height,(float)i/cmap.width);
					// 		glVertex3d(posi,posj+ajout,cmap.data[i][j+1].terrain_height/100.0);
					//
				    //         glTexCoord2f( (float)j/cmap.height, ((float)i+1)/cmap.width);
				    //         glVertex3d(posi+ajout,posj,cmap.data[i+1][j].terrain_height/100.0);
					//
				    //         // second triangle
				    //         glTexCoord2f( ((float)j+1)/cmap.height,((float)i+1)/cmap.width);
					// 		glVertex3d(posi+ajout,posj+ajout,cmap.data[i+1][j+1].terrain_height/100.0);
					// 	}
					// 	glEnd();
					// }

	for(i=0,posi=0 ;i<cmap.width-1;i++, posi += ajout)
	{
        glBegin(GL_TRIANGLE_STRIP);
		for(j=0, posj=0;j<cmap.height-1;j++, posj += ajout)
		{
			GLfloat deepness;
			deepness = (cmap.data[i][j].water_level + cmap.data[i][j].terrain_height) / (cmap.data[i][j].terrain_height+30.0f);
			// GLfloat alpha = 1.0f;
			if (cmap.data[i][j].water_level > 0)
				glColor4f(
					0,
					0.4f / deepness,
					0.9f / deepness,
					0.9f
				);
			else
				glColor4f(
					1.0f - 0.5f * deepness,
					1.0f - 0.25f * deepness,
					1.0f - 0.75f * deepness,
					1.0f
				);

            // premier triangle
			glTexCoord2f( (float)j / cmap.height, (float)i/cmap.width);
			glVertex3d(posi,posj,((cmap.data[i][j].water_level-1.0f)/1000.0 + cmap.data[i][j].terrain_height / 1000.0));

			glTexCoord2f( ((float)j+1)/cmap.height,(float)i/cmap.width);
			glVertex3d(posi,posj+ajout,((cmap.data[i][j+1].water_level-1.0f)/1000.0 + cmap.data[i][j+1].terrain_height / 1000.0));

            glTexCoord2f( (float)j/cmap.height, ((float)i+1)/cmap.width);
            glVertex3d(posi+ajout,posj,((cmap.data[i+1][j].water_level-1.0f)/1000.0 + cmap.data[i+1][j].terrain_height / 1000.0));

            // second triangle
            glTexCoord2f( ((float)j+1)/cmap.height,((float)i+1)/cmap.width);
			glVertex3d(posi+ajout,posj+ajout,((cmap.data[i+1][j+1].water_level-1.0f)/1000.0 + cmap.data[i+1][j+1].terrain_height / 1000.0));
		}
		glEnd();
	}







	glFlush();


	usleep(1000000 / FPS);
}
