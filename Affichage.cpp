#include "Affichage.hpp"
#include "general.hpp"
#include <GLUT/glut.h>

void	reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glScalef(1, -1, 1);
  glTranslatef(0, -h, 0);
}

void	display(void)
{
	static time_t	t1 = 0, t2 = 0;
	mtx.lock();
	if (q.size() < 1)
	{
		mtx.unlock();
		return ;
	}
	mtx.unlock();

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
  
	for (int x = 0 ; x < 1000 ; x++)
	{
  		for (int y = 0 ; y < 1000 ; y++)
		{
			glColor3f(1.0, 1.0, 1.0);
			glVertex2i(x, y);
		}
	}
	glEnd();

	Map cmap = q.front();

	glBegin(GL_POINTS);
	for (int x = 0 ; x < cmap.width ; x = x + 2)
	{
  		for (int y = 0 ; y < cmap.height ; y = y + 2)
		{
			if (cmap.data[x][y].water_level > 0)
			{
				float n = cmap.data[x][y].water_level / MAX_LVL_WATER * 2;
				n = (n > 1) ? 1 : n;
				glColor3f(0, 0, n + 0.5);
			}
			else
			{
				float n = cmap.data[x][y].terrain_height / MAX_LVL_TERRAIN * 2;
				n = (n > 1) ? 1 : n;
				glColor3f(n + 0.5, n + 0.5, 0);
			}
			glVertex2i(x*3,		y*3);
			glVertex2i(x*3-1,	y*3);
			glVertex2i(x*3-2,	y*3);
			glVertex2i(x*3,		y*3-1);
			glVertex2i(x*3-1,	y*3-1);
			glVertex2i(x*3-2,	y*3-1);
			glVertex2i(x*3,		y*3-2);
			glVertex2i(x*3-1,	y*3-2);
			glVertex2i(x*3-2,	y*3-2);
		}
	}
	glEnd();
	glFlush();  /* Single buffered, so needs a flush. */
	mtx.lock();
	q.pop();
	mtx.unlock();
	glutPostRedisplay();
	t2 = time(NULL);
	if (t2 != t1)
		t1 = t2;
	else
		usleep(1000000);
}
