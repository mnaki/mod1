#include "Affichage.hpp"
#include "general.hpp"
#include <stdio.h>

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

void reshape_vue_haute(int w, int h)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);       /* Establish viewing area to cover entire window. */
	glMatrixMode(GL_PROJECTION);  /* Start modifying the projection matrix. */
	glLoadIdentity();             /* Reset project matrix. */
	glOrtho(0, w, 0, h, -1, 1);   /* Map abstract coords directly to window coords. */
	glScalef(1, -1, 1);           /* Invert Y axis so increasing Y goes down. */
	glTranslatef(0, -h, 0);       /* Shift origin up to upper-left corner. */
}

void display_vue_haute(void)
{
    if (!running)
        exit(0);
	glClear(GL_COLOR_BUFFER_BIT);

	Map cmap;
	mtx.lock();
	if (conf_skip_frames)
		cmap = q.back();
	else
		cmap = q.front();
	mtx.unlock();

	int origin_x = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int origin_y = glutGet(GLUT_WINDOW_HEIGHT) / 20;

	glBegin(GL_POINTS);

	for (GLfloat x = 0; x < cmap.width - 1; x++)
	{
		for (GLfloat y = 0; y < cmap.height - 1; y++)
		{
			set_color(cmap, x, y);
			glVertex2i(origin_x - x + y, origin_y + x + y);
		}
	}

	glEnd();
	glutSwapBuffers();
}
