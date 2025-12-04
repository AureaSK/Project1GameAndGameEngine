#include "pch.h"
#include "CEngineRender.h"
#include "CEngineWindow.h"
#include <SDL3/SDL.h>

CEngineRender::CEngineRender()
    : sdlRenderer(nullptr)
{
}

CEngineRender::~CEngineRender()
{
    Destroy();
}

bool CEngineRender::Initialize(CEngineWindow* window)
{
    if (!window) {
        SDL_Log("Cannot create renderer: window is null");
        return false;
    }

    sdlRenderer = SDL_CreateRenderer(window->GetSDLWindow(), nullptr);

    if (!sdlRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    return true;
}

void CEngineRender::Destroy()
{
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }
}

void CEngineRender::Clear()
{
    SDL_RenderClear(sdlRenderer);
}

void CEngineRender::Present()
{
    SDL_RenderPresent(sdlRenderer);
}

void CEngineRender::SetDrawColor(const Color& color)
{
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
}

void CEngineRender::DrawRect(const SDL_FRect& rect, const Color& color)
{
    SetDrawColor(color);
    SDL_RenderFillRect(sdlRenderer, &rect);
}

void CEngineRender::DrawTexture(SDL_Texture* texture, const SDL_FRect* srcRect, const SDL_FRect* destRect, float rotation)
{
    if (texture) {
        SDL_RenderTextureRotated(sdlRenderer, texture, srcRect, destRect, rotation, nullptr, SDL_FLIP_NONE);
    }
}

SDL_Texture* CEngineRender::CreateTextureFromSurface(SDL_Surface* surface)
{
    if (!surface || !sdlRenderer) {
        return nullptr;
    }

    return SDL_CreateTextureFromSurface(sdlRenderer, surface);
}