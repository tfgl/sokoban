#include "renderer.h"

void keydown(SDL_Keycode k, GameState* game) {
  DIR d;

  switch(k) {
    case SDLK_COMMA:
      d = UP;
      break;

    case SDLK_a:
      d = LEFT;
      break;

    case SDLK_e:
      d = RIGHT;
      break;

    case SDLK_o:
      d = DOWN;
      break;

    case SDLK_SPACE:
      *game = initGame(game->baseMap);
      return;

    default:
      return;
  }

  move(&(game->player.p), d, game);
  game->player.rotation = d;
}

void keyup(SDL_Keycode k, GameState* game) {
}

void render(GameState* game, WindowStuff* win) {
  Player* player = &(game->player);
  int size = 96;

  for(int i = 0; i < H; i++) {
    for(int j = 0; j < W; j++) {
      SDL_Rect src, dst = {.x = j*size, .y = i*size, .w = size, .h = size};
      src.h = 32;
      src.w = 32;
      switch( game->map[i][j] ) {
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

      SDL_RenderCopy( win->renderer, win->textures, &src, &dst);

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
        SDL_RenderCopy( win->renderer, win->textures, &src, &dst);
      }
    }
  }

  SDL_RenderPresent( win->renderer );
}

void initWin(WindowStuff* win, int width, int height, char* title) {
  win->width = width;
  win->height = height;
  win->fps = 60;
  win->open = 1;

  win->viewPort.w = win->width;
  win->viewPort.h = win->height;
  SDL_Init(SDL_INIT_VIDEO);

  win->window = SDL_CreateWindow(
      title,
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      win->width,
      win->height,
      SDL_WINDOW_SHOWN);

  win->renderer = SDL_CreateRenderer(
      win->window,
      -1,
      SDL_RENDERER_ACCELERATED);

  SDL_Surface* surf = IMG_Load("textures.png");
  win->textures = SDL_CreateTextureFromSurface(win->renderer, surf);
  SDL_FreeSurface(surf);
}

void run(char map[H][W]) {
  WindowStuff win;
  GameState game = initGame(map);
  initWin(&win, 1000, 1000, "sokoban");

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
          keydown(e.key.keysym.sym, &game);
          break;

        case SDL_KEYUP:
          keyup(e.key.keysym.sym, &game);
          break;
      }
    }

    render(&game, &win);

    double delay = win.fps - (SDL_GetTicks() - t0);
    if(delay > 0)
      SDL_Delay(delay);
  }

  SDL_DestroyWindow(win.window);
  SDL_DestroyRenderer(win.renderer);
  SDL_DestroyTexture(win.textures);
  SDL_Quit();
}
