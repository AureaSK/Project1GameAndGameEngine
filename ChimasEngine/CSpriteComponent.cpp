#include "pch.h"
#include "CSpriteComponent.h"
#include "CWorld.h"
#include "CEngine.h"
#include "CEngineRender.h"

CSpriteComponent::CSpriteComponent(CActor* owner)
    : CComponent(owner), texture(nullptr), ownsTexture(false), useSourceRect(false),
    alpha(1.0f), tintColor(Color::White()), flipMode(SDL_FLIP_NONE)
{
    renderRect = { 0.0f, 0.0f, 64.0f, 64.0f };
    sourceRect = { 0.0f, 0.0f, 0.0f, 0.0f };
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
        SDL_Log("CSpriteComponent: No world available!");
        return false;
    }

    CEngine* engine = world->GetEngine();
    if (!engine || !engine->GetRenderer())
    {
        SDL_Log("CSpriteComponent: No engine or renderer available!");
        return false;
    }

    // ✅ ADD THIS: Resolve the asset path using engine's logic
    std::string resolvedPath = engine->ResolveAssetPath(path.c_str());

    texture = new CEngineTexture();
    if (!texture->LoadFromFile(resolvedPath))  // ✅ Use resolved path
    {
        SDL_Log("CSpriteComponent: Failed to load '%s'", resolvedPath.c_str());
        delete texture;
        texture = nullptr;
        return false;
    }

    if (!texture->CreateTexture(engine->GetRenderer()->GetSDLRenderer()))
    {
        SDL_Log("CSpriteComponent: Failed to create GPU texture");
        delete texture;
        texture = nullptr;
        return false;
    }

    ownsTexture = true;
    renderRect.w = static_cast<float>(texture->GetWidth());
    renderRect.h = static_cast<float>(texture->GetHeight());

    SDL_Log("CSpriteComponent: Loaded '%s' (%dx%d)",
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
    if (!texture || !texture->GetTexture() || !isActive)
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
    SDL_SetTextureAlphaMod(texture->GetTexture(), static_cast<Uint8>(alpha * 255));
    SDL_SetTextureColorMod(texture->GetTexture(), tintColor.r, tintColor.g, tintColor.b);

    // Render
    const SDL_FRect* srcRect = useSourceRect ? &sourceRect : nullptr;
    SDL_RenderTextureRotated(renderer, texture->GetTexture(),
        srcRect, &renderRect, rotation, nullptr, flipMode);
}

void CSpriteComponent::SetSize(float width, float height)
{
    renderRect.w = width;
    renderRect.h = height;
}

void CSpriteComponent::SetSourceRect(float x, float y, float w, float h)
{
    sourceRect = { x, y, w, h };
    useSourceRect = true;
}

void CSpriteComponent::ClearSourceRect()
{
    useSourceRect = false;
}