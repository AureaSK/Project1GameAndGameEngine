#pragma once

#include "CComponent.h"
#include "CEngineTexture.h"
#include "CActor.h"
//#include <SDL3/SDL.h>
#include <string>

class CSpriteComponent : public CComponent
{
private:
    CEngineTexture* texture;
    SDL_FRect renderRect;
    SDL_FRect sourceRect;  // For sprite sheets
    bool ownsTexture;
    bool useSourceRect;

    // Rendering properties
    float alpha;
    Color tintColor;
    SDL_FlipMode flipMode;

public:
    CSpriteComponent(CActor* owner);
    virtual ~CSpriteComponent();

    // Lifecycle
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void Destroy() override;

    // Texture loading - Gets renderer from engine automatically
    bool LoadTexture(const std::string& path);
    void SetTexture(CEngineTexture* tex);

    // Rendering - Called by engine, gets renderer from world/engine
    void Render(const Vector2& position, float rotation = 0.0f);

    // Size control
    void SetSize(float width, float height);
    void SetSourceRect(float x, float y, float w, float h);
    void ClearSourceRect();

    // Visual properties
    void SetAlpha(float a) { alpha = a; }
    void SetTint(const Color& color) { tintColor = color; }
    void SetFlip(SDL_FlipMode flip) { flipMode = flip; }

    // Getters
    CEngineTexture* GetTexture() const { return texture; }
    SDL_FRect GetRenderRect() const { return renderRect; }
    SDL_FRect GetSourceRect() const { return sourceRect; }
    float GetAlpha() const { return alpha; }
};

