#include "pch.h"
#include "SpaceshipPawn.h"
#include "PlayerController.h"
#include "Missile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "CHPComponent.h"
#include "CWorld.h"
#include "EnemyProjectile.h"
#include "Loner.h"
#include "Rusher.h"
#include "Drone.h"
#include "StoneAsteroidBig.h"
#include "StoneAsteroidMedium.h"
#include "StoneAsteroidSmall.h"
#include "GameManager.h"
#include "ChimasLog.h"

SpaceshipPawn::SpaceshipPawn(CWorld* world)
    : CPawn(world), sprite(nullptr), animation(nullptr), health(nullptr), gameManager(nullptr), moveSpeed(300.0f), fireRate(0.2f), fireCooldown(0.0f) { }

SpaceshipPawn::~SpaceshipPawn()
{
}

void SpaceshipPawn::BeginPlay()
{
    CPawn::BeginPlay();

    health = AddComponent<CHPComponent>();
    health->SetMaxHP(100.0f);

    if (gameManager)
    {
        gameManager->UpdateHealthDisplay(health->GetCurrentHP(), health->GetMaxHP());
    }

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
            ChimasLog::Info("Missile fired!");
            fireCooldown = fireRate;
        }
    }
}

void SpaceshipPawn::Destroy()
{
    isPendingKill = true;
    ChimasLog::Info("GAME OVER - Player destroyed!");

    // Notify game manager
    if (gameManager)
    {
        gameManager->OnPlayerDeath();
    }

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

    Rusher* rusher = dynamic_cast<Rusher*>(other);

    Drone* drone = dynamic_cast<Drone*>(other);

	StoneAsteroidBig* bigAsteroid = dynamic_cast<StoneAsteroidBig*>(other);

	StoneAsteroidMedium* mediumAsteroid = dynamic_cast<StoneAsteroidMedium*>(other);

	StoneAsteroidSmall* smallAsteroid = dynamic_cast<StoneAsteroidSmall*>(other);

    if (loner)
    {
        ChimasLog::Info("Player hit enemy");
        other->OnCollision(this);

        takenDamage = loner->GetDamageValue(takenDamage);

        health->ChangeHP(-takenDamage);
    }
    else if (eProject)
    {
        ChimasLog::Info("Player hit enemy projectile");
        other->OnCollision(this);

        takenDamage = eProject->GetDamageValue(takenDamage);

        health->ChangeHP(-takenDamage);
    }
    else if (rusher)
    {
        ChimasLog::Info("Player hit enemy");
        other->OnCollision(this);

        takenDamage = rusher->GetDamageValue(takenDamage);

        health->ChangeHP(-takenDamage);
    }
    else if (drone)
    {
        ChimasLog::Info("Player hit enemy");
        other->OnCollision(this);

        takenDamage = drone->GetDamageValue(takenDamage);

        health->ChangeHP(-takenDamage);
    }
	else if (bigAsteroid)
	{
		ChimasLog::Info("Player hit big asteroid");
		other->OnCollision(this);

        takenDamage = bigAsteroid->GetDamageValue(takenDamage);

		health->ChangeHP(-takenDamage);
	}
	else if (mediumAsteroid)
	{
		ChimasLog::Info("Player hit medium asteroid");
		other->OnCollision(this);

		takenDamage = mediumAsteroid->GetDamageValue(takenDamage);

		health->ChangeHP(-takenDamage);
	}
	else if (smallAsteroid)
	{
		ChimasLog::Info("Player hit small asteroid");
		other->OnCollision(this);

        takenDamage = smallAsteroid->GetDamageValue(takenDamage);

		health->ChangeHP(-takenDamage);
	}
	


    if (gameManager)
    {
        gameManager->UpdateHealthDisplay(health->GetCurrentHP(), health->GetMaxHP());
    }

    if (health->GetCurrentHP() <= 0.0f)
    {
        Destroy();
    }
}