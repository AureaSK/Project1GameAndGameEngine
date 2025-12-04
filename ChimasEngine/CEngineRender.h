#pragma once
#include <SDL3/SDL.h>
#include "ChimasUtilities.h"

// Forward declaration
class CEngineWindow;

// NO inheritance - Renderer is a standalone component
class CEngineRender
{
private:
    SDL_Renderer* sdlRenderer;

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

    // Texture rendering
    void DrawTexture(SDL_Texture* texture, const SDL_FRect* srcRect, const SDL_FRect* destRect, float rotation = 0.0f);

    // Texture creation helper
    SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);

    // Getter
    SDL_Renderer* GetSDLRenderer() const { return sdlRenderer; }
};