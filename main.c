#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

typedef struct Player {
  pos p;
  DIR rotation;
} Player;

void initWin(WindowStuff* win, int width, int height, char* title);

char map[H][W] = {
  {WALL, WALL  , WALL  , WALL  , WALL  , WALL  , WALL  , WALL  , WALL  , WALL},
  {WALL, TARGET, GROUND, GROUND, WALL  , GROUND, GROUND, GROUND, GROUND, WALL},
  {WALL, GROUND, CRATE , PLAYER, CRATE , CRATE , GROUND, TARGET, GROUND, WALL},
  {WALL, GROUND, GROUND, GROUND, WALL  , GROUND, GROUND, GROUND, GROUND, WALL},
  {WALL, GROUND, GROUND, GROUND, WALL  , GROUND, GROUND, GROUND, GROUND, WALL},
  {WALL, GROUND, GROUND, GROUND, WALL  , GROUND, GROUND, GROUND, GROUND, WALL},
  {WALL, GROUND, GROUND, GROUND, WALL  , GROUND, GROUND, GROUND, GROUND, WALL},
  {WALL, GROUND, GROUND, GROUND, GROUND, GROUND, GROUND, TARGET, GROUND, WALL},
  {WALL, GROUND, GROUND, GROUND, WALL  , GROUND, GROUND, GROUND, GROUND, WALL},
  {WALL, WALL  , WALL  , WALL  , WALL  , WALL  , WALL  , WALL  , WALL  , WALL}
};

void newPlayer(Player* player, int x, int y) {
  player->p[0] = x; player->p[1] = y;
  player->rotation = DOWN;
}

Player initMap(char map[H][W]) {
  Player p;
  for(int i = 0; i < H; i++) {
    for(int j = 0; j < W; j++) {
      if(map[i][j] == PLAYER) {
        newPlayer(&p, j, i);
        map[i][j] = GROUND;
      }
    }
  }
  return p;
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
        if( move(&p2, d) ) {
          (*p)[0] = p2[0];
          (*p)[1] = p2[1];
          return 1;
        }
        return 0;
        break;

      case LOCKED:
        if( move(&p2, d) ) {
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
    switch(*next) {
      case GROUND:
        *curent = GROUND;
        *next = CRATE;
        return 1;
        break;

      case CRATE:
      case LOCKED:
        if( move(&p2, d) ) {
          *curent = GROUND;
          *next = nextcpy;
          return 1;
        }
        return 0;
        break;

      case TARGET:
        *curent = GROUND;
        *next = LOCKED;
        return 1;
        break;
    }
  }
  return 0;
}

void render(Player* player, WindowStuff* win) {
  SDL_Surface* surf = IMG_Load("textures.png");
  SDL_Texture* textures = SDL_CreateTextureFromSurface(win->renderer, surf);
  int size = 96;

  for(int i = 0; i < H; i++) {
    for(int j = 0; j < W; j++) {
      SDL_Rect src, dst = {.x = j*size, .y = i*size, .w = size, .h = size};
      src.h = 32;
      src.w = 32;
      switch( map[i][j] ) {
        case GROUND:
          src.x = 32;
          src.y = 0;
          break;

        case WALL:
          src.x = 0;
          src.y = 0;
          break;

        case TARGET:
          src.x = 0;
          src.y = 64;
          break;

        case CRATE:
          src.x = 32;
          src.y = 64;
          break;

        case LOCKED:
          src.x = 64;
          src.y = 64;
          break;
      }

      SDL_RenderCopy( win->renderer, textures, &src, &dst);

      if( player->p[0] == j && player->p[1] == i) {
        switch( player->rotation ) {
          case UP:
            src.x = 32;
            src.y = 32;
            break;

          case LEFT:
            src.x = 64;
            src.y = 32;
            break;

          case DOWN:
            src.x = 64;
            src.y = 0;
            break;

          case RIGHT:
            src.x = 0;
            src.y = 32;
            break;
        }
        SDL_RenderCopy( win->renderer, textures, &src, &dst);
      }
    }
  }

  SDL_RenderPresent( win->renderer );
}

void keydown(SDL_Keycode k, Player* player) {
  switch(k) {
    case SDLK_COMMA:
      move(&player->p, UP);
      player->rotation = UP;
      break;

    case SDLK_a:
      move(&player->p, LEFT);
      player->rotation = LEFT;
      break;

    case SDLK_e:
      move(&player->p, RIGHT);
      player->rotation = RIGHT;
      break;

    case SDLK_o:
      move(&player->p, DOWN);
      player->rotation = DOWN;
      break;
  }
}

void keyup(SDL_Keycode k, Player* player) {
}

int main() {
  WindowStuff win;
  initWin(&win, 1000, 1000, "sokoban");
  Player player = initMap(map);

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
          keydown(e.key.keysym.sym, &player);
          break;

        case SDL_KEYUP:
          keyup(e.key.keysym.sym, &player);
          break;
      }
    }

    render(&player, &win);

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

