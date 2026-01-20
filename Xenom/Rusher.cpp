#include "pch.h"
#include "Rusher.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "Missile.h"
#include "CWorld.h"
#include "ChimasLog.h"

Rusher::Rusher(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), speed(100.0f), direction(1.f) {}

Rusher::~Rusher()
{

}

void Rusher::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/rusher.bmp"))
    {
        sprite->SetSize(64.0f, 64.0f);
    }

    // Add animation component 
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(64, 32);

    // Rusher animation (first row)
    animation->AddAnimation("move", {
        {0, 0, 0.1f}, {0, 1, 0.1f}, {0, 2, 0.1f}, {0, 3, 0.1f},
        {1, 0, 0.1f}, {1, 1, 0.1f}, {1, 2, 0.1f}, {1, 3, 0.1f},
        {2, 0, 0.1f}, {2, 1, 0.1f}, {2, 2, 0.1f}, {2, 3, 0.1f},
        {3, 0, 0.1f}, {3, 1, 0.1f}, {3, 2, 0.1f}, {3, 3, 0.1f},
        {4, 0, 0.1f}, {4, 1, 0.1f}, {4, 2, 0.1f}, {4, 3, 0.1f},
        {5, 0, 0.1f}, {5, 1, 0.1f}, {5, 2, 0.1f}, {5, 3, 0.1f}
        }, true);

    animation->PlayAnimation("move");

    // FIXED: Setup physics with proper collision
    physics = AddComponent<CPhysicsComponent>();

    physics->SetFixedRotation(true);

    // FIXED: Set collision filter BEFORE creating shape
    physics->SetCollisionFilter(
        CollisionCategory::ENEMY,
        CollisionCategory::PLAYER | CollisionCategory::PLAYER_PROJECTILE | CollisionCategory::WALL
    );

    // FIXED: Create as SOLID collision (sensor = false for physical collision)
    physics->CreateBoxShape(60.0f, 60.0f, false);

    ChimasLog::Info("Rusher spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void Rusher::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (physics)
    {
        physics->SetVelocity(Vector2(0.0f, speed * direction));
    }


    // Destroy if off screen
    if (transform.position.y > (world->GetWorldBounds().y - 50))
    {
        direction = -1.f;
    }
    if (transform.position.y < (0 + 50))
    {
        direction = 1.f;
    }
}

void Rusher::OnCollision(CActor* other)
{
    if (IsPendingKill() || !other || other->IsPendingKill()) return;
    Missile* missile = dynamic_cast<Missile*>(other);
    if (missile)
    {
        ChimasLog::Info("Rusher hit by missile!");
        Destroy();
        missile->OnCollision(this);
    }

}