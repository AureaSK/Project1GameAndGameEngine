#include "pch.h"
#include "CSpriteComponent.h"
#include "CWorld.h"
#include "CEngine.h"
#include "CEngineRender.h"
#include "ChimasLog.h"
#include <SDL3/SDL.h>

CSpriteComponent::CSpriteComponent(CActor* owner)
    : CComponent(owner), texture(nullptr), ownsTexture(false), useSourceRect(false),
    alpha(1.0f), tintColor(Color::White()), flipMode(FlipMode::None)
{
    renderRect = RectF(0.0f, 0.0f, 64.0f, 64.0f);
    sourceRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
}

CSpriteComponent::~CSpriteComponent()
{
    if (ownsTexture && texture)
    {
        delete texture;
    }
}

void CSpriteComponent::BeginPlay()
{
}

void CSpriteComponent::Tick(float deltaTime)
{
}

void CSpriteComponent::Destroy()
{
    if (ownsTexture && texture)
    {
        delete texture;
        texture = nullptr;
    }
}

bool CSpriteComponent::LoadTexture(const std::string& path)
{
    if (ownsTexture && texture)
    {
        delete texture;
    }

    // Get renderer from engine through world
    CWorld* world = owner->GetWorld();
    if (!world)
    {
        ChimasLog::Info("CSpriteComponent: No world available!");
        return false;
    }

    CEngine* engine = world->GetEngine();
    if (!engine || !engine->GetRenderer())
    {
        ChimasLog::Info("CSpriteComponent: No engine or renderer available!");
        return false;
    }

    // ✅ ADD THIS: Resolve the asset path using engine's logic
    std::string resolvedPath = engine->ResolveAssetPath(path.c_str());

    texture = new CEngineTexture();
    if (!texture->LoadFromFile(resolvedPath))  // ✅ Use resolved path
    {
        ChimasLog::Info("CSpriteComponent: Failed to load '%s'", resolvedPath.c_str());
        delete texture;
        texture = nullptr;
        return false;
    }

    if (!texture->CreateTexture(static_cast<void*>(engine->GetRenderer()->GetSDLRenderer())))
    {
        ChimasLog::Info("CSpriteComponent: Failed to create GPU texture");
        delete texture;
        texture = nullptr;
        return false;
    }

    ownsTexture = true;
    renderRect.w = static_cast<float>(texture->GetWidth());
    renderRect.h = static_cast<float>(texture->GetHeight());

    ChimasLog::Info("CSpriteComponent: Loaded '%s' (%dx%d)",
        resolvedPath.c_str(), texture->GetWidth(), texture->GetHeight());

    return true;
}

void CSpriteComponent::SetTexture(CEngineTexture* tex)
{
    if (ownsTexture && texture)
    {
        delete texture;
    }

    texture = tex;
    ownsTexture = false;

    if (texture)
    {
        renderRect.w = static_cast<float>(texture->GetWidth());
        renderRect.h = static_cast<float>(texture->GetHeight());
    }
}

void CSpriteComponent::Render(const Vector2& position, float rotation)
{
    if (!texture || !texture->GetNativeTexture() || !isActive)
    {
        return;
    }

    // Get renderer from engine
    CWorld* world = owner->GetWorld();
    if (!world) return;

    CEngine* engine = world->GetEngine();
    if (!engine || !engine->GetRenderer()) return;

    SDL_Renderer* renderer = engine->GetRenderer()->GetSDLRenderer();

    // Center sprite on actor position
    renderRect.x = position.x - (renderRect.w / 2.0f);
    renderRect.y = position.y - (renderRect.h / 2.0f);

    // Apply visual properties
    SDL_Texture* sdlTex = static_cast<SDL_Texture*>(texture->GetNativeTexture());
    SDL_SetTextureAlphaMod(sdlTex, static_cast<Uint8>(alpha * 255));
    SDL_SetTextureColorMod(sdlTex, tintColor.r, tintColor.g, tintColor.b);

    // Render (convert engine RectF -> SDL_FRect)
    SDL_FRect dst{ renderRect.x, renderRect.y, renderRect.w, renderRect.h };
    SDL_FRect src{ sourceRect.x, sourceRect.y, sourceRect.w, sourceRect.h };

    const SDL_FRect* srcPtr = useSourceRect ? &src : nullptr;

    SDL_FlipMode sdlFlip = SDL_FLIP_NONE;
    switch (flipMode)
    {
    case FlipMode::Horizontal: sdlFlip = SDL_FLIP_HORIZONTAL; break;
    case FlipMode::Vertical: sdlFlip = SDL_FLIP_VERTICAL; break;
    case FlipMode::None:
    default: sdlFlip = SDL_FLIP_NONE; break;
    }

    SDL_RenderTextureRotated(renderer, sdlTex,
        srcPtr, &dst, rotation, nullptr, sdlFlip);
}

void CSpriteComponent::SetSize(float width, float height)
{
    renderRect.w = width;
    renderRect.h = height;
}

void CSpriteComponent::SetSourceRect(float x, float y, float w, float h)
{
    sourceRect = RectF(x, y, w, h);
    useSourceRect = true;
}

void CSpriteComponent::ClearSourceRect()
{
    useSourceRect = false;
}