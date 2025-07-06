#include <iostream>
#include <SDL3/SDL.h>
#include <emscripten/emscripten.h>

SDL_Window* window;
SDL_Renderer* renderer;


void init() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout<<"failed to start sdl"<<std::endl;
        return;
    }

    std::cout<<"succ started sdl"<<std::endl;

    window = SDL_CreateWindow("canvas", 900, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(!window) {
        std::cout<<"error: "<<SDL_GetError()<<std::endl;
        return;
    }
    std::cout<<"connected to canvas"<<std::endl;

    renderer = SDL_CreateRenderer(window, nullptr);

    if(!renderer) {
            std::cout<<"error: "<<SDL_GetError()<<std::endl;
            return;
    }
        std::cout<<"created renderer"<<std::endl;
}
void tick() {


    SDL_FRect dstRect{0,0,800,600};

    SDL_SetRenderTarget(renderer, NULL);

    SDL_SetRenderDrawColor(renderer, 255,0,0,255);

    SDL_RenderRect(renderer, &dstRect);

    SDL_RenderPresent(renderer);

    SDL_Delay(50);

    return;
}


int main() {
    init();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, 1);
    #else

    #endif

    std::cout<<"done"<<std::endl;
}
