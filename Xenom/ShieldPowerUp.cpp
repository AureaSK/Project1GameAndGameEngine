#include "ShieldPowerUp.h"
#include "pch.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "SpaceshipPawn.h"
#include "CWorld.h"
#include "ChimasLog.h"

ShieldPowerUp::ShieldPowerUp(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), speed(150.0f), direction(1.f), heal(35.f) {}

ShieldPowerUp::~ShieldPowerUp()
{

}

void ShieldPowerUp::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/PUShield.bmp"))
    {
        sprite->SetSize(32.0f, 32.0f);
    }

    // Add animation component 
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(32, 32);

    // Big Asteroid animation (first row)
    animation->AddAnimation("move", {
        {0, 0, 0.1f}, {0, 1, 0.1f}, {0, 2, 0.1f}, {0, 3, 0.1f},
        {1, 0, 0.1f}, {1, 1, 0.1f}, {1, 2, 0.1f}, {1, 3, 0.1f}
        }, true);

    animation->PlayAnimation("move");

    // FIXED: Setup physics with proper collision
    physics = AddComponent<CPhysicsComponent>();

    physics->SetFixedRotation(true);

    // FIXED: Set collision filter BEFORE creating shape
    physics->SetCollisionFilter(
        CollisionCategory::POWERUP,
        CollisionCategory::PLAYER
    );

    // FIXED: Create as SOLID collision (sensor = false for physical collision)
    physics->CreateBoxShape(30.0f, 30.0f, false);

    ChimasLog::Info("Shield PowerUp spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void ShieldPowerUp::Tick(float deltaTime)
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

void ShieldPowerUp::OnCollision(CActor* other)
{
    if (IsPendingKill() || !other || other->IsPendingKill()) return;

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Heal PowerUp Caught!");
        Destroy();
        //Could Play a "PowerUp" VFX
    }
}

float ShieldPowerUp::GetDamageValue(float powerUpHeal)
{
    return heal;
}