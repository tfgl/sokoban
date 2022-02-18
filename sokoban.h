#pragma once
#ifndef _SOKOBAN_H_
#define _SOKOBAN_H_

#include <stdio.h>
#include <string.h>

#define W 10
#define H 10

#define WALL 'X'
#define CRATE 'c'
#define LOCKED '#'
#define TARGET 'o'
#define PLAYER 'p'
#define GROUND '.'

typedef enum {UP, LEFT, DOWN, RIGHT} DIR;
typedef int pos[2];

typedef struct Player {
  pos p;
  DIR rotation;
} Player;

typedef struct GameState {
  int targetLeft;
  char map[H][W];
  char baseMap[H][W];
  Player player;
} GameState;

void newPlayer(GameState* game, int x, int y);
GameState initGame(char map[H][W]);
pos* convertDir(DIR d);
char* nextTile(pos* p, DIR direction, GameState* game);
int move(pos* p, DIR d, GameState* game);
int checkForVictory();

#endif
