#include "pch.h"
#include "CImageWidget.h"
#include "CEngineTexture.h"
#include "CEngineRender.h"
#include "ChimasLog.h"
#include <SDL3/SDL.h>

CImageWidget::CImageWidget()
    : CWidget(), texture(nullptr), ownsTexture(false),
    tintColor(Color::White()), opacity(1.0f),
    sourceRect(0.0f, 0.0f, 0.0f, 0.0f), useSourceRect(false)
{
}

CImageWidget::~CImageWidget()
{
    if (ownsTexture && texture)
    {
        delete texture;
    }
}

bool CImageWidget::LoadImage(const std::string& path)
{
    if (ownsTexture && texture)
    {
        delete texture;
    }

    texture = new CEngineTexture();
    if (!texture->LoadFromFile(path))
    {
        ChimasLog::Info("CImageWidget: Failed to load image '%s'", path.c_str());
        delete texture;
        texture = nullptr;
        return false;
    }

    if (!texture->CreateTexture(nullptr))
    {
        ChimasLog::Info("CImageWidget: Failed to create GPU texture");
        delete texture;
        texture = nullptr;
        return false;
    }

    ownsTexture = true;

    // Auto-size the widget to match image dimensions
    size.x = static_cast<float>(texture->GetWidth());
    size.y = static_cast<float>(texture->GetHeight());

    ChimasLog::Info("CImageWidget: Loaded image '%s' (%dx%d)",
        path.c_str(), texture->GetWidth(), texture->GetHeight());

    return true;
}

void CImageWidget::SetTexture(CEngineTexture* tex)
{
    if (ownsTexture && texture)
    {
        delete texture;
    }

    texture = tex;
    ownsTexture = false;

    if (texture)
    {
        size.x = static_cast<float>(texture->GetWidth());
        size.y = static_cast<float>(texture->GetHeight());
    }
}

void CImageWidget::SetSourceRect(float x, float y, float w, float h)
{
    sourceRect = RectF(x, y, w, h);
    useSourceRect = true;
}

void CImageWidget::ClearSourceRect()
{
    useSourceRect = false;
}

void CImageWidget::Tick(float deltaTime)
{
    CWidget::Tick(deltaTime);
    // Add any image-specific logic here (animations, etc.)
}

void CImageWidget::Render(CEngineRender* renderer)
{
    if (!isVisible || !texture || !texture->GetNativeTexture())
    {
        return;
    }

    Vector2 pos = GetAbsolutePosition();

    // Source rectangle (what part of the texture to draw)
    SDL_FRect srcRect;
    if (useSourceRect)
    {
        srcRect.x = sourceRect.x;
        srcRect.y = sourceRect.y;
        srcRect.w = sourceRect.w;
        srcRect.h = sourceRect.h;
    }
    else
    {
        srcRect.x = 0.0f;
        srcRect.y = 0.0f;
        srcRect.w = static_cast<float>(texture->GetWidth());
        srcRect.h = static_cast<float>(texture->GetHeight());
    }

    // Destination rectangle (where to draw on screen)
    SDL_FRect dstRect;
    dstRect.x = pos.x;
    dstRect.y = pos.y;
    dstRect.w = size.x;
    dstRect.h = size.y;

    const SDL_FRect* srcPtr = useSourceRect ? &srcRect : nullptr;

    // Draw the image
    renderer->DrawTexture(
        texture->GetNativeTexture(),
        srcPtr,
        &dstRect,
        0.0f,
        texture->GetWidth(),
        texture->GetHeight()
    );

    // Note: Tint color and opacity are not yet implemented in OpenGL renderer
    // These can be added later by extending the shader system

    // Render children
    for (auto child : children)
    {
        if (child && child->IsVisible())
        {
            child->Render(renderer);
        }
    }
}