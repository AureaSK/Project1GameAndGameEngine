#include "pch.h"
#include "SpaceshipPawn.h"
#include "PlayerController.h"
#include "Missile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "CWorld.h"
#include "EnemyProjectile.h"
#include "Loner.h"
#include "Rusher.h"
#include "box2d.h"

SpaceshipPawn::SpaceshipPawn(CWorld* world)
    : CPawn(world), sprite(nullptr), animation(nullptr), moveSpeed(300.0f), fireRate(0.2f), fireCooldown(0.0f){}

SpaceshipPawn::~SpaceshipPawn()
{
}

void SpaceshipPawn::BeginPlay()
{
    CPawn::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/Ship1.bmp"))
    {
        sprite->SetSize(64.0f, 64.0f);
    }

    // Add animation component
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(64, 64);

    // Define animations
    animation->AddAnimation("idle", { {0, 3, 0.1f} }, true);
    animation->AddAnimation("left", {
        {0, 2, 0.1f},
        {0, 1, 0.1f},
        {0, 0, 0.1f}
        }, false);
    animation->AddAnimation("right", {
        {0, 4, 0.1f},
        {0, 5, 0.1f},
        {0, 6, 0.1f}
        }, false);

    animation->PlayAnimation("idle");

    // Add physics with proper collision setup
    physics = AddComponent<CPhysicsComponent>();
    physics->SetFixedRotation(true);

    // Set collision filter BEFORE creating shape
    physics->SetCollisionFilter(
        CollisionCategory::PLAYER,
        CollisionCategory::ENEMY | CollisionCategory::ENEMY_PROJECTILE | CollisionCategory::WALL
    );

    // Create as SOLID collision (sensor = false)
    physics->CreateBoxShape(50.0f, 50.0f, false);
}

void SpaceshipPawn::Tick(float deltaTime)
{
    CPawn::Tick(deltaTime);

    // Update fire cooldown
    if (fireCooldown > 0.0f)
    {
        fireCooldown -= deltaTime;
    }

    // NO INPUT HANDLING HERE - Controller handles that!
}

// Called BY the controller to move the pawn
void SpaceshipPawn::MoveInDirection(const Vector2& direction)
{
    Vector2 velocity = direction * moveSpeed;

    // Update animation based on horizontal movement
    if (direction.x < -0.1f)
    {
        if (!animation->IsAnimationPlaying("left"))
        {
            animation->PlayAnimation("left");
        }
    }
    else if (direction.x > 0.1f)
    {
        if (!animation->IsAnimationPlaying("right"))
        {
            animation->PlayAnimation("right");
        }
    }
    else
    {
        if (!animation->IsAnimationPlaying("idle"))
        {
            animation->PlayAnimation("idle");
        }
    }

    // Apply velocity to physics body
    if (physics)
    {
        physics->SetVelocity(velocity);
    }
}

// Called BY the controller to fire weapon
void SpaceshipPawn::FireWeapon()
{
    if (fireCooldown <= 0.0f)
    {
        Missile* missile = world->SpawnActor<Missile>();
        if (missile)
        {
            Vector2 spawnPos = transform.position;
            spawnPos.y -= 40.0f; // Spawn above ship
            missile->SetPosition(spawnPos);
            SDL_Log("Missile fired!");
            fireCooldown = fireRate;
        }
    }
}

void SpaceshipPawn::Destroy()
{
    isPendingKill = true;
    SDL_Log("GAME OVER - Player destroyed!");

    // Unpossess before destroying
    if (controller)
    {
        controller->UnPossess();
    }
}

void SpaceshipPawn::OnCollision(CActor* other)
{
    // Safety check - other might be pending kill
    if (!other || other->IsPendingKill()) return;

    EnemyProjectile* eProject = dynamic_cast<EnemyProjectile*>(other);
    Loner* loner = dynamic_cast<Loner*>(other);

    if (loner || eProject)
    {
        SDL_Log("Player hit enemy - GAME OVER!");
        other->OnCollision(this);
        Destroy();
    }
}