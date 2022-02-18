all: renderer.c renderer.h sokoban.c sokoban.h main.c
	gcc -o main -lSDL2 -lSDL2_image *.c
