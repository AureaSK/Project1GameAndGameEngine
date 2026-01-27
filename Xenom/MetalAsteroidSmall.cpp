#include "MetalAsteroidSmall.h"
#include "pch.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "SpaceshipPawn.h"
#include "Explosion.h"
#include "CWorld.h"
#include "ChimasLog.h"

MetalAsteroidSmall::MetalAsteroidSmall(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), speed(110.0f), direction(1.f), damage(15.f) {}

MetalAsteroidSmall::~MetalAsteroidSmall()
{

}

void MetalAsteroidSmall::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/MAster32.bmp"))
    {
        sprite->SetSize(32.0f, 32.0f);
    }

    // Add animation component 
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(32, 32);

    // Big Asteroid animation (first row)
    animation->AddAnimation("move", {
        {0, 0, 0.1f}, {0, 1, 0.1f}, {0, 2, 0.1f}, {0, 3, 0.1f}, {0, 4, 0.1f}, {0, 5, 0.1f}, {0, 6, 0.1f}, {0, 7, 0.1f},
        {1, 0, 0.1f}, {1, 1, 0.1f}, {1, 2, 0.1f}, {1, 3, 0.1f}, {1, 4, 0.1f}, {1, 5, 0.1f}, {1, 6, 0.1f}, {1, 7, 0.1f},
        }, true);

    animation->PlayAnimation("move");

    // FIXED: Setup physics with proper collision
    physics = AddComponent<CPhysicsComponent>();

    physics->SetFixedRotation(true);

    // FIXED: Set collision filter BEFORE creating shape
    physics->SetCollisionFilter(
        CollisionCategory::ENEMY,
        CollisionCategory::PLAYER | CollisionCategory::PLAYER_PROJECTILE
    );

    // FIXED: Create as SOLID collision (sensor = false for physical collision)
    physics->CreateBoxShape(30.0f, 30.0f, false);

    ChimasLog::Info("Small Metal Asteroid spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void MetalAsteroidSmall::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (physics)
    {
        physics->SetVelocity(Vector2(0.0f, speed * direction));
    }


    // Destroy if off screen
    if (transform.position.y > (world->GetWorldBounds().y + 150))
    {
        Destroy();
    }
}

void MetalAsteroidSmall::OnCollision(CActor* other)
{
    if (IsPendingKill() || !other || other->IsPendingKill()) return;

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Small Metal Asteroid hit spaceship!");

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }
    }
}

float MetalAsteroidSmall::GetDamageValue(float asteroidDamage)
{
    return damage;
}