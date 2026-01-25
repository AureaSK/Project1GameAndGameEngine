#include "pch.h"
#include "CFloatingTextWidget.h"
#include "CActor.h"

CFloatingTextWidget::CFloatingTextWidget()
    : CTextWidget(), targetActor(nullptr), offset(0.0f, -50.0f),
    lifetime(1.5f), maxLifetime(1.5f), floatSpeed(30.0f), shouldDestroy(false)
{
}

CFloatingTextWidget::~CFloatingTextWidget()
{
}

void CFloatingTextWidget::Tick(float deltaTime)
{
    CTextWidget::Tick(deltaTime);

    // Countdown lifetime
    lifetime -= deltaTime;

    if (lifetime <= 0.0f)
    {
        shouldDestroy = true;
        SetVisibility(false);
        return;
    }

    // Update position to follow target (if it exists)
    if (targetActor)
    {
        Vector2 targetPos = targetActor->GetPosition();

        // Float upward over time
        float currentOffset = offset.y - (maxLifetime - lifetime) * floatSpeed;

        SetPosition(Vector2(targetPos.x + offset.x, targetPos.y + currentOffset));
    }
    else
    {
        // No target - just float up from current position
        Vector2 currentPos = GetPosition();
        SetPosition(Vector2(currentPos.x, currentPos.y - floatSpeed * deltaTime));
    }
}

// Note: Render() is inherited from CTextWidget, which already has the ignoreScreenRotation fix