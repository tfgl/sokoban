#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define W 10
#define H 5

typedef enum {UP, LEFT, DOWN, RIGHT} DIR;
typedef int pos[2];
pos up    = { 0, -1};
pos left  = {-1,  0};
pos down  = { 0,  1};
pos right = { 1,  0};

typedef struct WindowStuff {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Rect viewPort;
  int width, height, fps;
  char* title;
  char open;
} WindowStuff;
void initWin(WindowStuff* win, int width, int height, char* title);

char map[H][W] = {
  {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
  {'X', '.', 'o', '.', 'X', '.', '.', '.', '.', 'X'},
  {'X', '.', 'c', '.', '.', '.', 'c', 'o', '.', 'X'},
  {'X', 'o', 'c', '.', 'X', '.', '.', '.', '.', 'X'},
  {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'}
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
  if( (*p)[0] == p2[0] && (*p)[1] == p2[1] && !((*next == 'o' && *curent == '.') || (*next == '.' && *curent == 'o'))) {
    *next = *curent;
    *curent = '.';
    return 1;
  }

  return 0;
}

void render(pos p, WindowStuff* win) {
  SDL_Surface* surf = IMG_Load("textures.png");
  SDL_Texture* textures = SDL_CreateTextureFromSurface(win->renderer, surf);
  int size = 96;

  for(int i = 0; i < H; i++) {
    for(int j = 0; j < W; j++) {
      SDL_Rect src, dst = {.x = j*size, .y = i*size, .w = size, .h = size};
      src.h = 32;
      src.w = 32;
      switch( map[i][j] ) {
        case '.':
          src.x = 32;
          src.y = 0;
          break;

        case 'X':
          src.x = 0;
          src.y = 0;
          break;

        case 'o':
          src.x = 0;
          src.y = 64;
          break;

        case 'c':
          src.x = 32;
          src.y = 64;
          break;

        case '#':
          src.x = 64;
          src.y = 64;
          break;
      }

      SDL_RenderCopy( win->renderer, textures, &src, &dst);

      if( p[0] == j && p[1] == i) {
        src.x = 64;
        src.y = 0;
        SDL_RenderCopy( win->renderer, textures, &src, &dst);
      }
    }
  }

  SDL_RenderPresent( win->renderer );
}

void keydown(SDL_Keycode k, pos* p) {
  switch(k) {
    case SDLK_COMMA:
      move(p, UP);
      break;

    case SDLK_a:
      move(p, LEFT);
      break;

    case SDLK_e:
      move(p, RIGHT);
      break;

    case SDLK_o:
      move(p, DOWN);
      break;
  }
}

void keyup(SDL_Keycode k, pos* p) {
}

int main() {
  WindowStuff win;
  initWin(&win, 1000, 1000, "sokoban");
  pos p;
  newPlayer(p, 3, 2);

  while( win.open ) {
    double t0 = SDL_GetTicks();
    SDL_RenderClear( win.renderer );

    SDL_Event e;
    while( SDL_PollEvent(&e) ) {
      switch( e.type ) {
        case SDL_QUIT:
          win.open = 0;
          break;

        case SDL_KEYDOWN:
          keydown(e.key.keysym.sym, &p);
          break;

        case SDL_KEYUP:
          keyup(e.key.keysym.sym, &p);
          break;
      }
    }

    render(p, &win);

    double delay = win.fps - (SDL_GetTicks() - t0);
    if(delay > 0)
      SDL_Delay(delay);
  }

  return 0;
}

void initWin(WindowStuff* win, int width, int height, char* title) {
  win->width = width;
  win->height = height;
  win->fps = 60;
  win->open = 1;
  win->title = malloc(sizeof(title));
  strcpy(win->title, title);

  win->viewPort.w = win->width;
  win->viewPort.h = win->height;
  SDL_Init(SDL_INIT_VIDEO);

  win->window = SDL_CreateWindow(
      win->title,
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      win->width,
      win->height,
      SDL_WINDOW_SHOWN);

  win->renderer = SDL_CreateRenderer(
      win->window,
      -1,
      SDL_RENDERER_ACCELERATED);
}
