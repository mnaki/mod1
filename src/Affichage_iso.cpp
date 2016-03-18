#include "Affichage.hpp"
#include "general.hpp"
#include <stdio.h>

void	setColor(float wl, float tl, float hr)
{
	float hauteur_ref = hr;
	float hauteur_ref2 = hr * 2.0f;

	GLfloat deepness = (wl + tl + 0.1f) / (tl + 5.0f);
	if (wl > 0.0f)
		glColor4f( 0.0f, 0.4f / deepness, 0.9f / deepness, 0.75f );
	else
	{
		float hauteur_map = tl;
		if (hauteur_map < hauteur_ref)
			glColor4f( 1.0f - hauteur_map / hauteur_ref, 1.0f, 1.0f - hauteur_map / hauteur_ref, 1.0f );
		else
		{
			float h = hauteur_map - hauteur_ref;
			glColor4f( 0.0f + h / hauteur_ref2, 1.0f - h / hauteur_ref2, 0.0f, 1.0f );
		}
	}
}

void reshape_iso(int w, int h)
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

void display_iso(void)
{
    if (!running)
        exit(0);
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	Map cmap;
	mtx.lock();
	if (q.size() <= 0)
	{
		mtx.unlock();
		std::cout << "no frame" << std::endl;
		return ;
	}
	else
	{
		cmap = q.front();

		if (conf_skip_frames)
			cmap = q.back();

		if (!conf_pause)
			q.pop();
		mtx.unlock();
	}


	static float hauteur_ref = -1.0f;
	if (hauteur_ref == -1.0f)
		hauteur_ref = cmap.get_hauteur_max() / 3.0f;

	int origin_x = w / 2;
	int origin_y = h / 20;

	glBegin(GL_POINTS);

	for (GLfloat x = 0; x < cmap.width - 2; x++)
	{
		for (GLfloat y = 0; y < cmap.height - 2; y++)
		{
			int tl = cmap.data[x][y].terrain_height;
			float wl = cmap.data[x][y].water_level;

			setColor(wl, tl, hauteur_ref);
			glVertex2i(origin_x - x + y, origin_y + x + y - (tl + wl));
		}
	}
	for (GLfloat x = 0; x < cmap.width - 1; x++)
	{
		GLfloat y = cmap.height - 2;
		int tl = cmap.data[x][y].terrain_height;
		float wl = cmap.data[x][y].water_level;
		for (float i = 0 ; i <= tl + wl ; i++)
		{
			if (i < tl || (i == tl && wl == 0))
				setColor(0, tl - i, hauteur_ref);
			else
				setColor(wl, tl, hauteur_ref);
			glVertex2i(origin_x - x + y, origin_y + x + y - i);
		}
	}
	for (GLfloat y = 0; y < cmap.height - 1; y++)
	{
		GLfloat x = cmap.width - 2;
		int tl = cmap.data[x][y].terrain_height;
		float wl = cmap.data[x][y].water_level;
		for (float i = 0 ; i <= tl + wl ; i++)
		{
			if (i < tl || (i == tl && wl == 0))
				setColor(0, tl - i, hauteur_ref);
			else
				setColor(wl, tl, hauteur_ref);
			glVertex2i(origin_x - x + y, origin_y + x + y - i);
		}
	}
	glEnd();

	glutSwapBuffers();
	idle();
}
