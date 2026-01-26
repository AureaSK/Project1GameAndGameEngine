#include "pch.h"
#include "Missile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "Explosion.h"
#include "Loner.h"
#include "Rusher.h"
#include "Drone.h"
#include "StoneAsteroidBig.h"
#include "StoneAsteroidMedium.h"
#include "StoneAsteroidSmall.h"
#include "CWorld.h"
#include "ChimasLog.h"

Missile::Missile(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr),
    physics(nullptr), speed(-400.0f), damage(25.f)
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
    Drone* drone = dynamic_cast<Drone*>(other);
    StoneAsteroidBig* bigAsteroid = dynamic_cast<StoneAsteroidBig*>(other);
    StoneAsteroidMedium* mediumAsteroid = dynamic_cast<StoneAsteroidMedium*>(other);
    StoneAsteroidSmall* smallAsteroid = dynamic_cast<StoneAsteroidSmall*>(other);

    if (loner || rusher || drone || bigAsteroid || mediumAsteroid || smallAsteroid)
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

float Missile::GetDamageValue(float missileDamage)
{
    return damage;
}
