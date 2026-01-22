#include "pch.h"
#include "Explosion.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "ChimasLog.h"

Explosion::Explosion(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr), lifetime(.6f)
{
}

Explosion::~Explosion()
{
}

void Explosion::BeginPlay()
{
    CActor::BeginPlay();

    sprite = AddComponent<CSpriteComponent>();
    sprite->LoadTexture("Xenom/ImagesForGame/explode64.bmp");
    sprite->SetSize(64.0f, 64.0f);

    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(64.f, 64.f);
    
    // Explosion animation (assuming 8 frames in first row)
    animation->AddAnimation("explode", {
        {0, 0, 0.06f}, {0, 1, 0.06f}, {0, 2, 0.06f}, {0, 3, 0.06f}, {0, 4, 0.06f},
        {1, 0, 0.06f}, {1, 1, 0.06f}, {1, 2, 0.06f}, {1, 3, 0.06f}, {1, 4, 0.06f},
        }, true);

    animation->PlayAnimation("explode");

    ChimasLog::Info("Explosion spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void Explosion::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    lifetime -= deltaTime;
    if (lifetime <= 0.0f)
    {
        Destroy();
    }
}