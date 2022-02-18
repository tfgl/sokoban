#include "sokoban.h"
pos up    = { 0, -1};
pos left  = {-1,  0};
pos down  = { 0,  1};
pos right = { 1,  0};

void newPlayer(GameState* game, int x, int y) {
  game->player.p[0] = x;
  game->player.p[1] = y;
  game->player.rotation = DOWN;
  game->map[y][x] = GROUND;
}

GameState initGame(char map[H][W]) {
  GameState game;
  game.targetLeft = 0;

  for(int i = 0; i < H; i++) {
    for(int j = 0; j < W; j++) {
      game.map[i][j] = map[i][j];
      game.baseMap[i][j] = map[i][j];

      if(map[i][j] == PLAYER)
        newPlayer(&game, j, i);

      else if(map[i][j] == TARGET)
        game.targetLeft++;
    }
  }

  return game;
}

pos* convertDir(DIR d) {
  switch(d) {
    case UP:
      return &up;
      break;

    case LEFT:
      return &left;
      break;

    case DOWN:
      return &down;
      break;

    case RIGHT:
      return &right;
      break;
  }
}

char* nextTile(pos* p, DIR direction, GameState* game) {
  int* d = *convertDir(direction);

  int x = (*p)[0], y = (*p)[1];
  int x2 = (*p)[0] + d[0], y2 = (*p)[1] + d[1];

  return &(game->map[y2][x2]);
}

int move(pos* p, DIR d, GameState* game) {
  char* next = nextTile(p, d, game);
  char* curent = &(game->map[(*p)[1]][(*p)[0]]);
  pos* v = convertDir(d);
  pos p2 = {(*p)[0] + (*v)[0], (*p)[1] + (*v)[1]};
  char nextcpy = *next;

  if(!(p2[0] < W && p2[0] >= 0 && p2[1] < H && p2[1] >= 0)) return 0;

  if( *curent == GROUND || *curent == TARGET) {
    switch(*next) {
      case TARGET:
      case GROUND:
        (*p)[0] = p2[0];
        (*p)[1] = p2[1];
        return 1;
        break;

      case CRATE:
        if( move(&p2, d, game) ) {
          (*p)[0] = p2[0];
          (*p)[1] = p2[1];
          return 1;
        }
        return 0;
        break;

      case LOCKED:
        if( move(&p2, d, game) ) {
          (*p)[0] = p2[0];
          (*p)[1] = p2[1];
          *next = TARGET;
          return 1;
        }
        return 0;
        break;
    }
  }
  if( *curent == CRATE || *curent == LOCKED ) {
    if( *curent == LOCKED )
      game->targetLeft++;

    switch(*next) {
      case GROUND:
        *curent = GROUND;
        *next = CRATE;
        return 1;
        break;

      case CRATE:
      case LOCKED:
        if( move(&p2, d, game) ) {
          *curent = GROUND;
          *next = nextcpy;
          if(nextcpy == LOCKED)
            game->targetLeft--;
          return 1;
        }
        return 0;
        break;

      case TARGET:
        *curent = GROUND;
        *next = LOCKED;
        game->targetLeft--;
        return 1;
        break;

      default:
        if( *curent == LOCKED )
          game->targetLeft--;
        break;
    }
  }
  return 0;
}

