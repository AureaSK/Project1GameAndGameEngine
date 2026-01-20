#include "pch.h"
#include "CEngineTexture.h"
#include <SDL3/SDL.h>

struct CEngineTexture::Impl
{
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;
};

CEngineTexture::CEngineTexture()
    : impl(new Impl()), width(0), height(0)
{
}

CEngineTexture::~CEngineTexture()
{
    Destroy();
    delete impl;
    impl = nullptr;
}

bool CEngineTexture::LoadFromFile(const std::string& path)
{
    // Clean up existing data
    Destroy();

    // Load BMP file
    impl->surface = SDL_LoadBMP(path.c_str());
    if (!impl->surface) {
        SDL_Log("Failed to load texture from '%s': %s", path.c_str(), SDL_GetError());
        return false;
    }

    width = impl->surface->w;
    height = impl->surface->h;

    // Set default color key (magenta for transparency)
    SetColorKey(255, 0, 255);

    SDL_Log("Loaded texture: %s (%dx%d)", path.c_str(), width, height);
    return true;
}

bool CEngineTexture::CreateTexture(void* nativeRenderer)
{
    SDL_Renderer* renderer = static_cast<SDL_Renderer*>(nativeRenderer);
    if (!impl || !impl->surface || !renderer) {
        SDL_Log("Cannot create texture: surface or renderer is null");
        return false;
    }

    // Destroy old texture if it exists
    if (impl->texture) {
        SDL_DestroyTexture(impl->texture);
    }

    // Create new texture from surface
    impl->texture = SDL_CreateTextureFromSurface(renderer, impl->surface);

    if (!impl->texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return false;
    }

    return true;
}

void CEngineTexture::SetColorKey(uint8_t r, uint8_t g, uint8_t b)
{
    if (impl && impl->surface) {
        SDL_SetSurfaceColorKey(impl->surface, true, SDL_MapSurfaceRGB(impl->surface, r, g, b));
    }
}

void CEngineTexture::Destroy()
{
    if (!impl) return;

    if (impl->texture) {
        SDL_DestroyTexture(impl->texture);
        impl->texture = nullptr;
    }

    if (impl->surface) {
        SDL_DestroySurface(impl->surface);
        impl->surface = nullptr;
    }

    width = 0;
    height = 0;
}

void* CEngineTexture::GetNativeTexture() const
{
    return impl ? static_cast<void*>(impl->texture) : nullptr;
}