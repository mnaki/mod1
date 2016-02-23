NAME = mod1

$(NAME):
	clear ; c++ -Wall -Wextra *.hpp *.cpp -std=gnu++11 -Ofast && ./a.out
