#pragma once
#include <SDL3/SDL.h>
#include "neck.h"
#include "data.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern Neck neck;

extern SDL_FRect dstRect;

extern SDL_FRect neckRect;

extern SDL_Texture* neckTexture;

extern std::vector<float> notes;

extern Song song;
