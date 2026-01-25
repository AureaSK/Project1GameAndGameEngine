#pragma once
#include <SDL3/SDL.h>
#include "ChimasUtilities.h"

// Forward declaration
class CEngineWindow;

// PIMPL to hide OpenGL/GLAD from game code
class CEngineRender
{
private:
    struct Impl;
    Impl* impl;

public:
    CEngineRender();
    ~CEngineRender();

    bool Initialize(CEngineWindow* window);
    void Destroy();

    // Drawing primitives
    void Clear();
    void Present();
    void SetDrawColor(const Color& color);
    void DrawRect(const SDL_FRect& rect, const Color& color);

    // Texture rendering (uses void* to hide OpenGL types)
    // textureWidth and textureHeight are needed for sprite sheet UV calculations
    void DrawTexture(void* texture, const SDL_FRect* srcRect, const SDL_FRect* destRect, float rotation = 0.0f, int textureWidth = 0, int textureHeight = 0, bool ignoreScreenRotation = false);

    // Texture creation helper
    void* CreateTextureFromSurface(SDL_Surface* surface);

    // Internal access (for CEngineTexture)
    void* GetInternalContext() const;
};