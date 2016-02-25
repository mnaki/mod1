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
    // mtx.lock();
    Map cmap = q.front();
    q.pop();
    // mtx.unlock();

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
	glFlush();
}
