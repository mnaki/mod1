#include "Affichage.hpp"
#include "general.hpp"

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

	// thread safe
	mtx.lock();
	if (q.size() < 1)
	{
		mtx.unlock();
		return ;
	}
	mtx.unlock();
	// end thread safe

	Map cmap = q.front();

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0 ; x < cmap.width ; x++)
	{
  		for (int y = 0 ; y < cmap.height ; y++)
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
			for (int i = 0 ; i < LARGEUR_PIXEL ; i++)
			{
				for (int j = 0 ; j < LONGEUR_PIXEL ; j++)
				{
					glVertex2i(x*LARGEUR_PIXEL + i, y*LONGEUR_PIXEL + j);
				}
			}
		}
	}
	glEnd();
	glFlush();  /* Single buffered, so needs a flush. */

	mtx.lock();
	q.pop();
	mtx.unlock();

	glutPostRedisplay();
	/*t2 = time(NULL);
	if (t2 != t1)
		t1 = t2;
	else
		usleep(1000000 / FPS);*/
}
