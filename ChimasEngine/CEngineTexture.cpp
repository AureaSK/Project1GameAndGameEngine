#include "pch.h"
#include "CEngineTexture.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>

// PIMPL Implementation
struct CEngineTexture::Impl
{
    SDL_Surface* surface;
    GLuint textureID;
    uint8_t colorKeyR;
    uint8_t colorKeyG;
    uint8_t colorKeyB;
    bool hasColorKey;

    Impl() : surface(nullptr), textureID(0), colorKeyR(255),
        colorKeyG(0), colorKeyB(255), hasColorKey(false) {
    }
};

CEngineTexture::CEngineTexture()
    : impl(new Impl())
    , width(0)
    , height(0)
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
    Destroy();

    impl->surface = SDL_LoadBMP(path.c_str());
    if (!impl->surface) {
        SDL_Log("Failed to load texture from '%s': %s", path.c_str(), SDL_GetError());
        return false;
    }

    width = impl->surface->w;
    height = impl->surface->h;

    SetColorKey(255, 0, 255);

    SDL_Log("Loaded texture: %s (%dx%d)", path.c_str(), width, height);
    return true;
}

bool CEngineTexture::CreateTexture(void* nativeRenderer)
{
    if (!impl || !impl->surface) {
        SDL_Log("Cannot create texture: surface is null");
        return false;
    }

    if (impl->textureID) {
        glDeleteTextures(1, &impl->textureID);
        impl->textureID = 0;
    }

    if (impl->hasColorKey) {
        SDL_SetSurfaceColorKey(impl->surface, true,
            SDL_MapSurfaceRGB(impl->surface, impl->colorKeyR, impl->colorKeyG, impl->colorKeyB));
    }

    SDL_Surface* rgbaSurface = SDL_ConvertSurface(impl->surface, SDL_PIXELFORMAT_RGBA32);
    if (!rgbaSurface) {
        SDL_Log("Failed to convert surface to RGBA: %s", SDL_GetError());
        return false;
    }

    glGenTextures(1, &impl->textureID);
    glBindTexture(GL_TEXTURE_2D, impl->textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgbaSurface->w, rgbaSurface->h,
        0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_DestroySurface(rgbaSurface);

    SDL_Log("Created OpenGL texture ID: %u", impl->textureID);
    return true;
}

void CEngineTexture::SetColorKey(uint8_t r, uint8_t g, uint8_t b)
{
    if (impl) {
        impl->colorKeyR = r;
        impl->colorKeyG = g;
        impl->colorKeyB = b;
        impl->hasColorKey = true;

        if (impl->surface) {
            SDL_SetSurfaceColorKey(impl->surface, true,
                SDL_MapSurfaceRGB(impl->surface, r, g, b));
        }
    }
}

void CEngineTexture::Destroy()
{
    if (!impl) return;

    if (impl->textureID) {
        glDeleteTextures(1, &impl->textureID);
        impl->textureID = 0;
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
    return impl ? reinterpret_cast<void*>(static_cast<uintptr_t>(impl->textureID)) : nullptr;
}