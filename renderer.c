#include "renderer.h"

void keydown(SDL_Keycode k, GameState* game) {
  DIR d;

  switch(k) {
    case SDLK_UP:
      d = UP;
      break;

    case SDLK_LEFT:
      d = LEFT;
      break;

    case SDLK_RIGHT:
      d = RIGHT;
      break;

    case SDLK_DOWN:
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

void renderText(WindowStuff* win, const char* txt, int x, int y, int scale, int centered) {
  int fontSize = 4;
  int numbersSize = 7;
  if( centered ) x -= (strlen(txt) - 1) * fontSize * scale;

  for(int i=0; txt[i] != 0; i++) {
    char c = txt[i];
    if( c >= '0' && c <= ':') {
      c -= '0';
      SDL_Rect src = {c * numbersSize, 0, numbersSize, numbersSize},
               dst = {x + scale * fontSize * i, y, fontSize*scale, fontSize*scale};
      SDL_RenderCopy(win->renderer, win->numbers, &src, &dst);
    }
    else {
      if( c >= 'a') c -= 'a' - 'A';
      if( c == ' ')
        c = 26;
      else
        c -= 'A';

      SDL_Rect src = {c * fontSize, 0, fontSize, fontSize},
               dst = {x + scale * fontSize * i, y, fontSize*scale, fontSize*scale};
      SDL_RenderCopy(win->renderer, win->letters, &src, &dst);
    }
  }
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

  if ( game->targetLeft == 0 ) {
    int c = size * 2;
    SDL_Rect src = {.x=0, .y=0, .w=32, .h=32};
    SDL_Rect dst = {.x=c, .y=c, .w=win->width-c*2, .h=win->height-c*2};
    SDL_RenderCopy( win->renderer, win->winScreen, &src, &dst);
  }

  char targetleft[32];
  sprintf(targetleft, "left : %d", game->targetLeft);
  renderText(win, targetleft, 0, 0, 6, 0);
  renderText(win, SDL_GetWindowTitle(win->window), win->width/2, 0, 6, 1);

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

  SDL_Surface *texsurf = IMG_Load("textures.png"),
              *winsurf = IMG_Load("winscreen.png"),
              *lettersurf = IMG_Load("letters.png"),
              *numbersuf = IMG_Load("numbers.png");

  win->textures = SDL_CreateTextureFromSurface(win->renderer, texsurf);
  win->winScreen = SDL_CreateTextureFromSurface(win->renderer, winsurf);
  win->numbers = SDL_CreateTextureFromSurface(win->renderer, numbersuf);
  win->letters = SDL_CreateTextureFromSurface(win->renderer, lettersurf);
  SDL_FreeSurface(texsurf);
  SDL_FreeSurface(winsurf);
  SDL_FreeSurface(numbersuf);
  SDL_FreeSurface(lettersurf);
}

void run(char map[H][W]) {
  WindowStuff win;
  GameState game = initGame(map);
  initWin(&win, W*96, H*96, "sokoban");

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
  SDL_DestroyTexture(win.winScreen);
  SDL_Quit();
}
