#include "pch.h"
#include "CEngineTexture.h"

CEngineTexture::CEngineTexture()
    : surface(nullptr), sdlTexture(nullptr), width(0), height(0)
{
}

CEngineTexture::~CEngineTexture()
{
    Destroy();
}

bool CEngineTexture::LoadFromFile(const std::string& path)
{
    // Clean up existing data
    Destroy();

    // Load BMP file
    surface = SDL_LoadBMP(path.c_str());
    if (!surface) {
        SDL_Log("Failed to load texture from '%s': %s", path.c_str(), SDL_GetError());
        return false;
    }

    width = surface->w;
    height = surface->h;

    // Set default color key (magenta for transparency)
    SetColorKey(255, 0, 255);

    SDL_Log("Loaded texture: %s (%dx%d)", path.c_str(), width, height);
    return true;
}

bool CEngineTexture::CreateTexture(SDL_Renderer* renderer)
{
    if (!surface || !renderer) {
        SDL_Log("Cannot create texture: surface or renderer is null");
        return false;
    }

    // Destroy old texture if it exists
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
    }

    // Create new texture from surface
    sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!sdlTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return false;
    }

    return true;
}

void CEngineTexture::SetColorKey(uint8_t r, uint8_t g, uint8_t b)
{
    if (surface) {
        SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, r, g, b));
    }
}

void CEngineTexture::Destroy()
{
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
        sdlTexture = nullptr;
    }

    if (surface) {
        SDL_DestroySurface(surface);
        surface = nullptr;
    }

    width = 0;
    height = 0;
}