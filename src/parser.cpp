#include <iostream>
#include <fstream>
#include "parser.hpp"

#define WIDTH 200
#define HEIGHT 200

Map *init_from_fichier(std::string s)
{
	std::ifstream fichier(s, std::ios::in);

	if (fichier)
	{
		std::string contenu;
		Map *map;

		getline(fichier, contenu); // scenario

		try {map = new Map(WIDTH, HEIGHT, std::stoi(contenu));}
		catch (std::invalid_argument) {std::cout << "19" << std::endl;exit(1);}

		getline(fichier, contenu, ' '); // x

		while (fichier.eof() == false)
		{
			int x, y, radius, height;

			try {x = std::stoi(contenu);}
			catch (std::invalid_argument) {std::cout << "30" << std::endl;exit(1);}
			
			getline(fichier, contenu, ' '); // y
			try {y = std::stoi(contenu);}
			catch (std::invalid_argument) {std::cout << "34" << std::endl;exit(1);}
			
			getline(fichier, contenu, ' '); // radius
			try {radius = std::stoi(contenu);}
			catch (std::invalid_argument) {std::cout << "38" << std::endl;exit(1);}
			
			getline(fichier, contenu, '\n'); // hauteur
			try {height = std::stoi(contenu);}
			catch (std::invalid_argument) {std::cout << "42" << std::endl;exit(1);}
		
			if (height < 0)
			{
				std::cout << x << "	" << y << "	" << radius << "	" << height << std::endl;
				map->draw_cone(x, y, radius, height * -1, true);
			}
			else
			{
				std::cout << x << "	" << y << "	" << radius << "	" << height << std::endl;
				map->draw_cone(x, y, radius, height, false);
			}
		
			getline(fichier, contenu, ' '); // x
		}

		fichier.close();
		return map;
	}
	else
		std::cerr << "Impossible d'ouvrir le fichier '" << s << "'" << std::endl;
	return NULL;
}
