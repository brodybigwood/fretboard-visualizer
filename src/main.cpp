#include <iostream>
#include <SDL3/SDL.h>
#include <emscripten/emscripten.h>
#include "global.h"



void init() {

    dstRect = SDL_FRect{0,0,1000,150};

    neckRect = SDL_FRect{10,10,dstRect.w-20,dstRect.h-20};

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout<<"failed to start sdl"<<std::endl;
        return;
    }

    std::cout<<"succ started sdl"<<std::endl;

    window = SDL_CreateWindow("canvas", dstRect.w, dstRect.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

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

    neckTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        neckRect.w,
        neckRect.h
    );
}
void tick() {

    SDL_SetRenderTarget(renderer, NULL);

    SDL_SetRenderDrawColor(renderer, 70,70,70,255);
    SDL_RenderClear(renderer);


    SDL_RenderTexture(renderer, neck.boardTexture, NULL, &neckRect);
    SDL_RenderTexture(renderer, neck.stringTexture, NULL, &neckRect);

    std::vector<float> notes;
    notes.push_back(69);
    notes.push_back(72);
    notes.push_back(74);
    notes.push_back(76);
    notes.push_back(79);
    neck.renderNotes(notes);

    SDL_RenderPresent(renderer);

    SDL_Delay(50);

    return;
}


void setup() {
    neck.width = 2; //2 inches
    neck.length = 19.125; //neck length inches, corresponds to 3/4 25.5 scale length
    neck.dstRect = &neckRect;

    //8 string tritone tuning
    neck.strings.push_back(22);
    neck.strings.push_back(28);
    neck.strings.push_back(34);
    neck.strings.push_back(40);
    neck.strings.push_back(46);
    neck.strings.push_back(52);
    neck.strings.push_back(58);
    neck.strings.push_back(64);

    neck.generateTextures();
}


int main() {
    init();
    setup();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, 1);
    #else

    #endif

    std::cout<<"done"<<std::endl;
}
