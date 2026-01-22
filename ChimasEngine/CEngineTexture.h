#pragma once

#include "pch.h"
#include "CEngine.h"
#include <string>

// NO inheritance - Texture is a resource holder
class CEngineTexture : public CEngine
{
private:
    // Hide SDL_Surface/SDL_Texture from game code (PIMPL)
    struct Impl;
    Impl* impl;

    int width;
    int height;

public:
    CEngineTexture();
    ~CEngineTexture();

    // Load from file (creates surface only)
    bool LoadFromFile(const std::string& path);

    // Create SDL_Texture from loaded surface
    bool CreateTexture(void* nativeRenderer);

    void Destroy();

    // Getters
    void* GetNativeTexture() const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    // Color key for transparency
    void SetColorKey(uint8_t r, uint8_t g, uint8_t b);
};