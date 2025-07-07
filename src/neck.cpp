#include "global.h"
#include <cmath>
#include <iostream>
#include <algorithm>

void renderThickLine(SDL_Renderer* renderer, float x1, float y1, float x2, float y2) {

}

void Neck::generateTextures() {

    boardTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        dstRect->w,
        dstRect->h
    );

    stringTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        dstRect->w,
        dstRect->h
    );
    renderBoard();
    renderStrings();
}

void Neck::renderBoard() {
    SDL_SetRenderTarget(renderer, boardTexture);

    float scaleLength = boardTexture->w * 4.0f/3;
    float xCenter = boardTexture->w /2.0f;
    float yCenter = boardTexture->h / 2.0f;

    bool drawMarkers = true;

    if(drawMarkers)
    { // fret marker block with variable radius
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        const int markerFrets[] = {3, 5, 7, 9, 12, 15, 17, 19, 21};
        float radius = 5.0f;

        for (int i = 0; i < sizeof(markerFrets) / sizeof(int); ++i) {
            float fret = markerFrets[i];
            float x1 = scaleLength - (scaleLength / std::pow(2.0f, (fret - 1) / 12.0f));
            float x2 = scaleLength - (scaleLength / std::pow(2.0f, fret / 12.0f));
            float cx = (x1 + x2) / 2.0f;
            float cy = yCenter;

            SDL_FRect marker = {cx - radius, cy - radius, radius * 2.0f, radius * 2.0f};
            SDL_RenderFillRect(renderer, &marker);
        }
    }


    SDL_SetRenderDrawColor(renderer,150,150,150,255);

    float fretRadius = boardTexture->h / 2.0f - 0.5f;

    SDL_RenderLine(renderer, 0, yCenter + fretRadius,
                   boardTexture->w, yCenter + fretRadius);
    SDL_RenderLine(renderer, 0, yCenter - fretRadius,
                   boardTexture->w, yCenter - fretRadius);

    for(float fret = 0; fret <= 24; fret += 1.0f) {
        float x = scaleLength - (scaleLength / std::pow(2.0f, fret / 12.0f));
        x = std::min(x, static_cast<float>(boardTexture->w - 1));
        SDL_RenderLine(renderer, x, yCenter + fretRadius, x, yCenter - fretRadius);
    }
}

void Neck:: renderStrings() {
    SDL_SetRenderTarget(renderer, stringTexture);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    float xCenter = boardTexture->w /2.0f;
    float yCenter = boardTexture->h / 2.0f;

    float fretRadius = boardTexture->h / 2.0f;

    float spacing = boardTexture->h /(strings.size() + 1.0f); //6 strings

    float y = yCenter - fretRadius + spacing;
    for(auto string : strings) {
        SDL_RenderLine(renderer, 0, y, boardTexture->w, y);
        y += spacing;
    }
}

std::vector<SDL_FPoint> Neck::getNotePositions(float midiNum) {
    std::vector<SDL_FPoint> positions;

    float scaleLength = length * 4/3.0f;
    float stringSpacing = dstRect->h / (strings.size() + 1.0f);

    for (size_t i = 0; i < strings.size(); ++i) {
        float openNote = strings[i];
        float diff = midiNum - openNote;
        float y = stringSpacing * (i + 1);

        if (diff < 0) continue;

        // Iterate over octaves, calculating fret as float
        for (int octave = 0; octave <= diff / 12; ++octave) {
            float fret = diff - 12.0f * static_cast<float>(octave);
            if (fret >= 0.0f && fret <= 24.0f) {
                float x = scaleLength - (scaleLength / std::pow(2.0f, fret / 12.0f));
                positions.push_back(SDL_FPoint{x, y});
            }
        }
    }
    return positions;
}



void Neck::renderNoteDot(float x, float y, float radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_FRect dotRect = { x - radius, y - radius, radius * 2, radius * 2 };
    SDL_RenderFillRect(renderer, &dotRect);

}

void Neck::renderNotes(std::vector<float>& nums) {

    float radius = 5.0f;
    SDL_Color color = {255,0,0,255};

    for (auto midiNum : nums) {
        std::vector<SDL_FPoint> positions = getNotePositions(midiNum);
        for (const SDL_FPoint& pos : positions) {
            // Map from neck texture coords to dstRect screen coords
            float screenX = dstRect->x + pos.x * (dstRect->w / length);
            float screenY = dstRect->y + pos.y;
            renderNoteDot(screenX, screenY, radius, color);
        }
    }
}

