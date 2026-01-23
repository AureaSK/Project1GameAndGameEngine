#pragma once
#include "pch.h"
#include "CEngine.h"
#include <string>

// OpenGL-based texture (PIMPL hides GLAD completely)
class CEngineTexture : public CEngine
{
private:
    struct Impl;
    Impl* impl;

    int width;
    int height;

public:
    CEngineTexture();
    ~CEngineTexture();

    // Load from file (creates surface only)
    bool LoadFromFile(const std::string& path);

    // Create OpenGL texture from loaded surface
    bool CreateTexture(void* nativeRenderer);

    void Destroy();

    // Getters
    void* GetNativeTexture() const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    // Color key for transparency
    void SetColorKey(uint8_t r, uint8_t g, uint8_t b);
};