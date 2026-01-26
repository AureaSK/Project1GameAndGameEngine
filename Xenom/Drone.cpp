#include "pch.h"
#include "Drone.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "Missile.h"
#include "SpaceshipPawn.h"
#include "Explosion.h"
#include "CWorld.h"
#include "ChimasLog.h"

Drone::Drone(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), speed(100.0f), direction(1.f) {}

Drone::~Drone()
{

}

void Drone::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/drone.bmp"))
    {
        sprite->SetSize(64.0f, 64.0f);
    }

    // Add animation component 
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(32, 32);

    // Rusher animation (first row)
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
        CollisionCategory::PLAYER | CollisionCategory::PLAYER_PROJECTILE | CollisionCategory::WALL
    );

    // FIXED: Create as SOLID collision (sensor = false for physical collision)
    physics->CreateBoxShape(60.0f, 60.0f, false);

    ChimasLog::Info("Rusher spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void Drone::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    // Accumulate time for sine wave
    timeAccumulator += deltaTime;

    if (physics)
    {
        // Set vertical velocity only
        physics->SetVelocity(Vector2(0.0f, speed * direction));

        // Set horizontal position directly using sine wave
        float effectiveTime = timeAccumulator - timeOffset;
        float xPos = baseX + sin(effectiveTime * sineFrequency) * sineAmplitude;

        transform.position.x = xPos;
        physics->SetPosition(transform.position);

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

void Drone::OnCollision(CActor* other)
{
    if (IsPendingKill() || !other || other->IsPendingKill()) return;

    Missile* missile = dynamic_cast<Missile*>(other);
    if (missile)
    {
        ChimasLog::Info("Drone hit by missile!");
        Destroy();
        missile->OnCollision(this);
    }

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Drone hit spaceship!");
        Destroy();

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }

		// DEAL DAMAGE TO PLAYER
    }

}