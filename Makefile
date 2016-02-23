# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bgauci <bgauci@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/02/23 20:57:43 by bgauci            #+#    #+#              #
#    Updated: 2016/02/23 21:01:26 by bgauci           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = mod1

INCDIR   = inc
SRCDIR   = src
OBJDIR   = obj

# all flags
FLAGS_INCLUDES = -I$(INCDIR)
FLAGS_OPTI = -Ofast
CFLAGS = $(FLAGS_ERROR) $(FLAGS_INCLUDES) -std=gnu++11 $(FLAGS_OPTI)
LFLAGS = $(CFLAGS) -framework GLUT -framework Opengl

FLAGS_ERROR =	-Wall -Wextra

SRC =		$(SRCDIR)/main.cpp				\
			$(SRCDIR)/Affichage.cpp			\
			$(SRCDIR)/Map.class.cpp			\
			$(SRCDIR)/MapPoint.class.cpp

OBJ =		$(OBJDIR)/main.o				\
			$(OBJDIR)/Affichage.o			\
			$(OBJDIR)/Map.class.o			\
			$(OBJDIR)/MapPoint.class.o

OBJECTS  := $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
CC = c++
RMF = rm -f

all: $(NAME)

$(NAME): depend $(OBJECTS)
	$(CC) -o $(NAME) $(OBJ) $(LFLAGS)

gdb:
	$(CC) -o $(NAME) $(SRC) $(CFLAGS) -g

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RMF) $(OBJ)

fclean: clean
	$(RMF) -r $(NAME).dSYM
	$(RMF) $(NAME)

clear: clean
re: fclean all

depend:
#	@makedepend -Y. -I$(LIBDIR) $(SRC) 2>/dev/null
#	@$(RMF) Makefile.bak

.PHONY: gdb depend clear clean fclean re all
