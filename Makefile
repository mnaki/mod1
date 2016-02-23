NAME = mod1

$(NAME):
	clear ;
	c++ -Wall -Wextra  *.cpp -std=gnu++11 -Ofast -framework GLUT -framework Opengl
	./a.out
