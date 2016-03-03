#include <iostream>
#include <fstream>
#include "parser.hpp"

Map *init_from_fichier(std::string s)
{
	std::ifstream fichier(s, std::ios::in);

	if (fichier)
	{
		std::string contenu;
		int width = 0, height = 0;

		getline(fichier, contenu); // width
		width = std::stoi(contenu);
		std::cout << width << std::endl;

		getline(fichier, contenu); // height
		height = std::stoi(contenu);
		std::cout << height << std::endl;

		getline(fichier, contenu); // scenario
		std::cout << contenu << std::endl;

		Map *map = new Map(width, height, contenu);

		getline(fichier, contenu, ' '); // start_x
		std::cout << "[" << contenu << "]" << std::endl;

		while (fichier.eof() == false)
		{
			int start_x, start_y, radius, height;

			start_x = std::stoi(contenu);
			getline(fichier, contenu, ' '); // start_y
			std::cout << contenu << std::endl;
			start_y = std::stoi(contenu);
			getline(fichier, contenu, ' '); // radius
			std::cout << contenu << std::endl;
			radius = std::stoi(contenu);
			getline(fichier, contenu, ' '); // height
			std::cout << contenu << std::endl;
			height = std::stoi(contenu);
			getline(fichier, contenu);		// reverse (bool)
			std::cout << contenu << std::endl;
			if (contenu == "true")
				map->draw_cone(start_x, start_y, radius, height, true);
			else
				map->draw_cone(start_x, start_y, radius, height, false);
		
			getline(fichier, contenu, ' '); // start_x
			std::cout << "[" << contenu << "]" << std::endl;
		}

		fichier.close();
		return map;
	}
	else
		std::cerr << "Impossible d'ouvrir le fichier '" << s << "'" << std::endl;
	return NULL;
}
