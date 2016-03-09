# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bgauci <bgauci@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/02/23 20:57:43 by bgauci            #+#    #+#              #
#    Updated: 2016/03/09 22:40:46 by bgauci           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = mod1

INCDIR   = inc
SRCDIR   = src
OBJDIR   = obj

# all flags
FLAGS_INCLUDES = -I$(INCDIR)
FLAGS_OPTI = -O3
CFLAGS = $(FLAGS_ERROR) $(FLAGS_INCLUDES) -std=gnu++11 $(FLAGS_OPTI) $(DEFS)
ifeq ($(DEFS),-D__LINUX__)
                LFLAGS = $(CFLAGS) -lGL -lglut -lGLU -lX11 -lm -lrt -lpthread
else ifeq ($(DEFS),-D__WIN__)
                LFLAGS = $(CFLAGS) -LC:/freeglut/lib/ -lfreeglut -lopengl32 -lpthread -lglu32
                CFLAGS += -IC:/freeglut/include/
else
                LFLAGS = $(CFLAGS) -framework GLUT -framework Opengl
endif

FLAGS_ERROR =	-Wall -Wextra -Wno-deprecated-declarations


SRC =		$(SRCDIR)/main.cpp				\
			$(SRCDIR)/gestion_input.cpp		\
			$(SRCDIR)/Affichage_iso.cpp		\
			$(SRCDIR)/Affichage_haut.cpp	\
			$(SRCDIR)/Affichage_3D.cpp		\
			$(SRCDIR)/Map.class.cpp			\
			$(SRCDIR)/parser.cpp			\
			$(SRCDIR)/MapPoint.class.cpp

OBJ =		$(OBJDIR)/main.o				\
			$(OBJDIR)/gestion_input.o		\
			$(OBJDIR)/Affichage_iso.o		\
			$(OBJDIR)/Affichage_haut.o		\
			$(OBJDIR)/Affichage_3D.o		\
			$(OBJDIR)/Map.class.o			\
			$(OBJDIR)/parser.o				\
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

linux_install:
	sudo apt-get install freeglut3 freeglut3-dev

linux:
	make DEFS=-D__LINUX__

clear: clean
re: fclean all

depend:
#	@makedepend -Y. -I$(LIBDIR) $(SRC) 2>/dev/null
#	@$(RMF) Makefile.bak

.PHONY: gdb depend clear clean fclean re all
