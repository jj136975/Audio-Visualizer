##
## EPITECH PROJECT, 2020
## makefile
## File description:
## CPool D10
##

NAME	=	sound_visualizer

SRC 	=	src/main.c \
			kissfft/*.c

OBJ	= 	$(SRC:.c=.o)

CC 	= 	gcc

CFLAGS 	=	-W -Wall -Wextra -I./include -I./kissfft

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ) -lm -lcsfml-graphics -lcsfml-window -lcsfml-system -lcsfml-audio

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all
