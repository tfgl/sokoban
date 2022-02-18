#pragma once
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "sokoban.h"

typedef struct WindowStuff {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Rect viewPort;
  int width, height, fps;
  char open;
  SDL_Texture* textures;
  SDL_Texture* winScreen;
  SDL_Texture* numbers;
  SDL_Texture* letters;
} WindowStuff;

void initWin(WindowStuff* win, int width, int height, char* title);
void render(GameState* game, WindowStuff* win);
void keydown(SDL_Keycode k, GameState* game);
void keyup(SDL_Keycode k, GameState* game);
void render(GameState* game, WindowStuff* win);
void run(char map[H][W]);
void renderText(WindowStuff* win, const char* txt, int x, int y, int scale, int centered);

#endif
