#pragma once
#include "CTextWidget.h"

class CActor;

class CFloatingTextWidget : public CTextWidget
{
private:
    CBitmapFont* font;
    std::string text;
    float scale;

    CActor* targetActor;
    Vector2 offset;

    // Animation properties
    float lifetime;
    float maxLifetime;
    float floatSpeed;  // How fast it moves up
    bool shouldDestroy;

public:
    CFloatingTextWidget();
    virtual ~CFloatingTextWidget();

    void SetTarget(CActor* actor) { targetActor = actor; }
    void SetOffset(const Vector2& off) { offset = off; }
    void SetLifetime(float time) { maxLifetime = time; lifetime = time; }
    void SetFloatSpeed(float speed) { floatSpeed = speed; }

    bool ShouldDestroy() const { return shouldDestroy; }

    // Lifecycle overrides
    virtual void Tick(float deltaTime) override;
    virtual void Render(CEngineRender* renderer) override;  // Added: Override to ignore screen rotation
};