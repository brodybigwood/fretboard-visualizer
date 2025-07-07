#pragma once
#include <SDL3/SDL.h>
#include <vector>

class Neck {
    public:
        float width;
        float length; //from fret 0 to last fret

        std::vector<float> strings;

        SDL_FRect* dstRect; //rectangle to render into;

        SDL_Texture* boardTexture;
        SDL_Texture* stringTexture;

        SDL_Texture* texture;

        void renderBoard();
        void renderStrings();

        void renderNoteDot(float, float, float, SDL_Color);
        void renderNotes(std::vector<float>&);

        std::vector<SDL_FPoint> getNotePositions(float);

        void generateTextures();
};
