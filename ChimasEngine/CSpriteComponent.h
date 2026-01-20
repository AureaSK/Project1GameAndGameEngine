#pragma once

#include "CComponent.h"
#include "CEngineTexture.h"
#include "CActor.h"
#include "ChimasUtilities.h"
#include <string>

class CSpriteComponent : public CComponent
{
private:
    CEngineTexture* texture;
    RectF renderRect;
    RectF sourceRect;  // For sprite sheets
    bool ownsTexture;
    bool useSourceRect;

    // Rendering properties
    float alpha;
    Color tintColor;
    FlipMode flipMode;

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
    void SetFlip(FlipMode flip) { flipMode = flip; }

    // Getters
    CEngineTexture* GetTexture() const { return texture; }
    RectF GetRenderRect() const { return renderRect; }
    RectF GetSourceRect() const { return sourceRect; }
    float GetAlpha() const { return alpha; }
};

