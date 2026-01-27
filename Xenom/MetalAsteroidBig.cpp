#include "MetalAsteroidBig.h"
#include "pch.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "SpaceshipPawn.h"
#include "Explosion.h"
#include "CWorld.h"
#include "ChimasLog.h"

MetalAsteroidBig::MetalAsteroidBig(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), speed(60.0f), direction(1.f), damage(45.f){}

MetalAsteroidBig::~MetalAsteroidBig()
{

}

void MetalAsteroidBig::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/MAster96.bmp"))
    {
        sprite->SetSize(96.0f, 96.0f);
    }

    // Add animation component 
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(96, 96);

    // Big Asteroid animation (first row)
    animation->AddAnimation("move", {
        {0, 0, 0.1f}, {0, 1, 0.1f}, {0, 2, 0.1f}, {0, 3, 0.1f}, {0, 4, 0.1f},
        {1, 0, 0.1f}, {1, 1, 0.1f}, {1, 2, 0.1f}, {1, 3, 0.1f}, {1, 4, 0.1f},
        {2, 0, 0.1f}, {2, 1, 0.1f}, {2, 2, 0.1f}, {2, 3, 0.1f}, {2, 4, 0.1f},
        {3, 0, 0.1f}, {3, 1, 0.1f}, {3, 2, 0.1f}, {3, 3, 0.1f}, {3, 4, 0.1f},
        {4, 0, 0.1f}, {4, 1, 0.1f}, {4, 2, 0.1f}, {4, 3, 0.1f}, {4, 4, 0.1f},
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
    physics->CreateBoxShape(70.0f, 70.0f, false);

    ChimasLog::Info("Big Metal Asteroid spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void MetalAsteroidBig::Tick(float deltaTime)
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

void MetalAsteroidBig::OnCollision(CActor* other)
{
    if (IsPendingKill() || !other || other->IsPendingKill()) return;

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Big Metal Asteroid hit spaceship!");

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }
    }
}

float MetalAsteroidBig::GetDamageValue(float asteroidDamage)
{
    return damage;
}