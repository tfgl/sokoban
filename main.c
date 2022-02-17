#include <stdio.h>
#include <malloc.h>
#define W 5
#define H 5

typedef enum {UP, LEFT, DOWN, RIGHT} DIR;
typedef int pos[2];
pos up    = { 0, -1};
pos left  = {-1,  0};
pos down  = { 0,  1};
pos right = { 1,  0};

char map[H][W] = {
  {'.', '.', '.', '.', '.'},
  {'.', 'X', '.', '.', '.'},
  {'.', '.', 'c', '.', '.'},
  {'.', 'o', 'c', '.', '.'},
  {'.', '.', '.', '.', '.'}
};

void newPlayer(pos p, int x, int y) {
  p[0] = x; p[1] = y;
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

char* nextTile(pos p, DIR direction) {
  switch(direction) {
    case UP:
      return &map[p[1] - 1][p[0]];
      break;

    case LEFT:
      return &map[p[1]][p[0] - 1];
      break;

    case DOWN:
      return &map[p[1] + 1][p[0]];
      break;

    case RIGHT:
      return &map[p[1]][p[0] + 1];
      break;
  }
}

int move(pos* p, DIR d) {
  char* next = nextTile(*p, d);
  char* curent = &map[(*p)[1]][(*p)[0]];
  pos* v = convertDir(d);
  pos p2 = {(*p)[0] + (*v)[0], (*p)[1] + (*v)[1]};
  pos p3 = {p2[0], p2[1]};

  //printf("(%d, %d): [%c] -> (%d, %d): [%c]\n", (*p)[0], (*p)[1], *curent, p2[0], p2[1], *next);
  // bounding check
  if(!(p2[0] < W && p2[0] >= 0 && p2[1] < H && p2[1] >= 0)) return 0;

  switch(*next) {
    case '.':
      (*p)[0] = p2[0];
      (*p)[1] = p2[1];
      break;

    case 'c':
      if(move(&p3, d)) {
        (*p)[0] = p2[0];
        (*p)[1] = p2[1];
      }
      break;

    case 'X':
      break;

    case 'o':
      (*p)[0] = p2[0];
      (*p)[1] = p2[1];
      if( *curent == 'c' )
        *curent = '#';
      break;
  }
  // if moved successfully
  if( (*p)[0] == p2[0] && (*p)[1] == p2[1] ) {
    *next = *curent;
    *curent = '.';
    return 1;
  }

  return 0;
}

void render(pos* p) {
  for(int i = 0; i < H; i++) {
    for(int j = 0; j < W; j++) {
      if( (*p)[0] == j && (*p)[1] == i)
        putchar('P');
      else
        putchar( map[i][j] );
    }
    putchar('\n');
  }
  putchar('\n');
}

int main() {
  pos* p = malloc(sizeof(pos));
  newPlayer(*p, 3, 2);
  render(p);
  move(p, UP);
  render(p);
  move(p, LEFT);
  render(p);
  move(p, DOWN);
  render(p);
  move(p, RIGHT);
  move(p, DOWN);
  render(p);
  move(p, LEFT);
  move(p, LEFT);
  move(p, UP);
  render(p);
  return 0;
}
