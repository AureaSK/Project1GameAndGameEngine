#include "pch.h"
#include "Spaceship.h"
#include "Missile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "CInputManager.h"
#include "CWorld.h"
#include "EnemyProjectile.h"
#include "Loner.h"
#include "Rusher.h"
#include "ChimasInput.h"
#include "ChimasLog.h"

Spaceship::Spaceship(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr), moveSpeed(300.0f), fireRate(0.2f), fireCooldown(0.0f) {
}

Spaceship::~Spaceship()
{
}

void Spaceship::BeginPlay()
{
    CActor::BeginPlay();


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

    // FIXED: Add physics with proper collision setup
    physics = AddComponent<CPhysicsComponent>();

    physics->SetFixedRotation(true);

    // FIXED: Set collision filter BEFORE creating shape
    physics->SetCollisionFilter(
        CollisionCategory::PLAYER,
        CollisionCategory::ENEMY | CollisionCategory::ENEMY_PROJECTILE | CollisionCategory::WALL
    );

    
   
    // FIXED: Create as SOLID collision (sensor = false)
    physics->CreateBoxShape(50.0f, 50.0f, false);
    ChimasLog::Info("Spaceship initialized at (%.1f, %.1f)", transform.position.x, transform.position.y);

}

void Spaceship::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    CInputManager* input = CInputManager::GetInstance();
    Vector2 velocity(0.0f, 0.0f);

    // Horizontal movement
    if (input->IsKeyDown(Key::Left) || input->IsKeyDown(Key::A))
    {
        velocity.x = -moveSpeed;
        if (!animation->IsAnimationPlaying("left"))
        {
            animation->PlayAnimation("left");
        }
    }
    else if (input->IsKeyDown(Key::Right) || input->IsKeyDown(Key::D))
    {
        velocity.x = moveSpeed;
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

    // Vertical movement
    if (input->IsKeyDown(Key::Up) || input->IsKeyDown(Key::W))
    {
        velocity.y = -moveSpeed;
    }
    else if (input->IsKeyDown(Key::Down) || input->IsKeyDown(Key::S))
    {
        velocity.y = moveSpeed;
    }

    // Gamepad support
    float axisX = input->GetAxis(GamepadAxis::LeftX);
    float axisY = input->GetAxis(GamepadAxis::LeftY);

    if (std::abs(axisX) > 0.2f)
    {
        velocity.x = axisX * moveSpeed;
        if (axisX < 0 && !animation->IsAnimationPlaying("left"))
        {
            animation->PlayAnimation("left");
        }
        else if (axisX > 0 && !animation->IsAnimationPlaying("right"))
        {
            animation->PlayAnimation("right");
        }
    }

    if (std::abs(axisY) > 0.2f)
    {
        velocity.y = axisY * moveSpeed;
    }

    // Apply velocity to physics body
    if (physics)
    {
        physics->SetVelocity(velocity);
    }

    // Fire cooldown
    if (fireCooldown > 0.0f)
    {
        fireCooldown -= deltaTime;
    }

    // Fire missiles
    if ((input->IsKeyPressed(Key::Space) ||
        input->IsButtonPressed(GamepadButton::South)) &&
        fireCooldown <= 0.0f)
    {
        FireMissile();
        fireCooldown = fireRate;
    }
}

void Spaceship::Destroy()
{
    isPendingKill = true;
    ChimasLog::Info("GAME OVER - Player destroyed!");

    // Call base class destroy
    //CActor::Destroy();

    // End the game when player is destroyed
   // if (world)
    //{
       // world->GameRunning = false;
        
    //}
}

void Spaceship::OnCollision(CActor* other)
{
    // Safety check - other might be pending kill
    if (!other || other->IsPendingKill()) return;

    EnemyProjectile* eProject = dynamic_cast<EnemyProjectile*>(other);

    Loner* loner = dynamic_cast<Loner*>(other);
    if (loner || eProject)
    {
        ChimasLog::Info("Player hit enemy - GAME OVER!");
        
        other->OnCollision(this);

        Destroy();
    }
}


void Spaceship::FireMissile()
{
    Missile* missile = world->SpawnActor<Missile>();
    if (missile)
    {
        Vector2 spawnPos = transform.position;
        spawnPos.y -= 40.0f; // Spawn above ship
        missile->SetPosition(spawnPos);
        ChimasLog::Info("Missile fired!");
    }
}