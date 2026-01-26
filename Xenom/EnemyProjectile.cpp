#include "pch.h"
#include "EnemyProjectile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "Explosion.h"
#include "SpaceshipPawn.h"
#include "CWorld.h"
#include "ChimasLog.h"

EnemyProjectile::EnemyProjectile(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr),
    physics(nullptr), speed(250.f), damage(25.f)
{
}

EnemyProjectile::~EnemyProjectile()
{

}

void EnemyProjectile::BeginPlay()
{
    CActor::BeginPlay();

    sprite = AddComponent<CSpriteComponent>();
    sprite->LoadTexture("Xenom/ImagesForGame/EnWeap6.bmp");
    sprite->SetSize(16.0f, 16.0f);

    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(16, 16);

    animation->AddAnimation("fly", {
        {0, 0, 0.1f},
        {0, 1, 0.1f},
        {0, 2, 0.1f},
        {0, 3, 0.1f}
        }, true);
    animation->PlayAnimation("fly");

    // Enemy projectile - collides ONLY with player (not enemies, not walls)
    physics = AddComponent<CPhysicsComponent>();

    physics->SetCollisionFilter(
        CollisionCategory::ENEMY_PROJECTILE,
        CollisionCategory::PLAYER | CollisionCategory::PLAYER_PROJECTILE
    );

    physics->CreateCircleShape(6.0f, false);

    ChimasLog::Info("Enemy projectile spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void EnemyProjectile::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (physics)
    {
        physics->SetVelocity(Vector2(0.0f, speed));
    }

    // Destroy if off screen
    if (transform.position.y > 620.0f)
    {
        Destroy();
    }
}

void EnemyProjectile::OnCollision(CActor* other)
{
    // Check if we hit the player
    SpaceshipPawn* spaceship = dynamic_cast<SpaceshipPawn*>(other);

    if (spaceship)
    {
        ChimasLog::Info("Enemy projectile hit player!");

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }

        Destroy();
    }
    else
    {
        Destroy();
    }
}

float EnemyProjectile::GetDamageValue(float projectileDamage)
{
    return damage;
}