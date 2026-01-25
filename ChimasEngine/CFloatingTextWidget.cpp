#include "pch.h"
#include "CFloatingTextWidget.h"
#include "CActor.h"
#include "CEngineRender.h"
#include "CEngineTexture.h"
#include <SDL3/SDL.h>

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

void CFloatingTextWidget::Render(CEngineRender* renderer)
{
    if (!isVisible || !font || !font->GetTexture() || text.empty())
        return;

    CEngineTexture* texture = font->GetTexture();
    if (!texture->GetNativeTexture())
        return;

    Vector2 renderPos = GetAbsolutePosition();

    // Calculate alignment offset
    Vector2 textSize = MeasureText();

    if (horizontalAlignment == TextAlign::Center)
    {
        renderPos.x += (size.x - textSize.x) / 2.0f;
    }
    else if (horizontalAlignment == TextAlign::Right)
    {
        renderPos.x += size.x - textSize.x;
    }

    if (verticalAlignment == TextAlign::Center)
    {
        renderPos.y += (size.y - textSize.y) / 2.0f;
    }
    else if (verticalAlignment == TextAlign::Right)
    {
        renderPos.y += size.y - textSize.y;
    }

    float cursorX = renderPos.x;
    float cursorY = renderPos.y;

    // Render each character
    for (char c : text)
    {
        if (c == '\n')
        {
            cursorX = renderPos.x;
            cursorY += font->GetLineHeight() * scale;
            continue;
        }

        const CharacterData* charData = font->GetCharacter(c);
        if (!charData)
            continue;

        SDL_FRect srcRect;
        srcRect.x = static_cast<float>(charData->x);
        srcRect.y = static_cast<float>(charData->y);
        srcRect.w = static_cast<float>(charData->width);
        srcRect.h = static_cast<float>(charData->height);

        SDL_FRect dstRect;
        dstRect.x = cursorX + (charData->xOffset * scale);
        dstRect.y = cursorY + (charData->yOffset * scale);
        dstRect.w = charData->width * scale;
        dstRect.h = charData->height * scale;

        // CRITICAL FIX: Pass true for ignoreScreenRotation to keep floating text upright
        renderer->DrawTexture(
            texture->GetNativeTexture(),
            &srcRect,
            &dstRect,
            0.0f,
            texture->GetWidth(),
            texture->GetHeight(),
            true  // <-- This keeps the floating text upright!
        );

        cursorX += charData->xAdvance * scale;
    }

    // Render children
    for (auto child : children)
    {
        if (child && child->IsVisible())
        {
            child->Render(renderer);
        }
    }
}