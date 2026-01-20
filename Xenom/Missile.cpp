#include "pch.h"
#include "Missile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "Explosion.h"
#include "Loner.h"
#include "Rusher.h"
#include "CWorld.h"
#include "ChimasLog.h"

Missile::Missile(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr),
    physics(nullptr), speed(-400.0f)  // Negative = upward
{
}

Missile::~Missile()
{

}

void Missile::BeginPlay()
{
    CActor::BeginPlay();

    sprite = AddComponent<CSpriteComponent>();
    sprite->LoadTexture("Xenom/ImagesForGame/missile.bmp");
    sprite->SetSize(16.0f, 16.0f);

    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(16,16);

    animation->AddAnimation("fly", {
        {0, 0, 0.1f},
        {0, 1, 0.1f},
        }, true);
    animation->PlayAnimation("fly");

    // Player projectile - collides ONLY with enemies (not player, not walls)
    physics = AddComponent<CPhysicsComponent>();

    physics->SetFixedRotation(true);


    physics->SetCollisionFilter(
        CollisionCategory::PLAYER_PROJECTILE,
        CollisionCategory::ENEMY | CollisionCategory::ENEMY_PROJECTILE | CollisionCategory::WALL // Only collide with enemies
    );

    physics->CreateCircleShape(6.0f, false);

    ChimasLog::Info("Missile spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void Missile::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (physics)
    {
        physics->SetVelocity(Vector2(0.0f, speed));
    }

    // Destroy if off screen (top)
    if (transform.position.y < -20.0f)
    {
        Destroy();
    }
}

void Missile::OnCollision(CActor* other)
{
    Loner* loner = dynamic_cast<Loner*>(other);
    Rusher* rusher = dynamic_cast<Rusher*>(other);

    if (loner || rusher)
    {
        ChimasLog::Info("Missile hit enemy!");

        // Destroy the enemy
        //other->OnCollision(this);

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }

        // Destroy the missile
        Destroy();
    }
}