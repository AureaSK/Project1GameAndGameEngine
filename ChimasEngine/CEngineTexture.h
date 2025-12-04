#pragma once

#include "pch.h"
#include "CEngine.h"
#include <string>
#include <SDL3/SDL.h>

// NO inheritance - Texture is a resource holder
class CEngineTexture : public CEngine
{
private:
    SDL_Surface* surface;
    SDL_Texture* sdlTexture;
    int width;
    int height;

public:
    CEngineTexture();
    ~CEngineTexture();

    // Load from file (creates surface only)
    bool LoadFromFile(const std::string& path);

    // Create SDL_Texture from loaded surface
    bool CreateTexture(SDL_Renderer* renderer);

    void Destroy();

    // Getters
    SDL_Surface* GetSurface() const { return surface; }
    SDL_Texture* GetTexture() const { return sdlTexture; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    // Color key for transparency
    void SetColorKey(uint8_t r, uint8_t g, uint8_t b);
};