#include "Affichage.hpp"
#include "general.hpp"
#include <stdio.h>

GLfloat rotate =			GLF_ROTATE;

static void set_color(Map const & cmap, int x, int y)
{
	static float hauteur_ref = -1.0f;
	static float hauteur_ref2 = -1.0f;
	if (hauteur_ref == -1.0f)
	{
		hauteur_ref = cmap.get_hauteur_max() / 3.0f;
		hauteur_ref2 = hauteur_ref * 2.0f;
	}

	GLfloat deepness = (cmap.data[x][y].water_level + cmap.data[x][y].terrain_height + 0.1f) / (cmap.data[x][y].terrain_height + 5.0f);
	if (cmap.data[x][y].water_level > 0.0f)
		glColor4f( 0.0f, 0.4f / deepness, 0.9f / deepness, 0.75f );
	else
	{
		float hauteur_map = cmap.data[x][y].terrain_height;
		if (hauteur_map < hauteur_ref)
			glColor4f( 1.0f - hauteur_map / hauteur_ref, 1.0f, 1.0f - hauteur_map / hauteur_ref, 1.0f );
		else
		{
			float h = hauteur_map - hauteur_ref;
			glColor4f( 0.0f + h / hauteur_ref2, 1.0f - h / hauteur_ref2, 0.0f, 1.0f );
		}
	}
}

void reshape(int w, int h)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background

	glMatrixMode(GL_PROJECTION);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	gluPerspective(90.0f, w / h, 1.0f, h);
	glMatrixMode(GL_MODELVIEW);
	glRotatef(45.0f, -205.0f, 0, 0);
	glTranslatef(0, 0,-265.0f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	mtx.lock();
	if (q.size() <= 0)
	{
		mtx.unlock();
		std::cout << "no frame" << std::endl;
		return ;
	}

	Map cmap = q.front();

	if (conf_skip_frames)
		cmap = q.back();

	if (!conf_pause)
		q.pop();

	mtx.unlock();

	if (conf_rotate)
		rotate -= 45.0f/8.0f;

	rotate += (target_rotate + - rotate) / 8.0f;

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

	glutSwapBuffers();
	idle();
}
