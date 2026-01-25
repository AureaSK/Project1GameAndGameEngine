#include "pch.h"
#include "Loner.h"
#include "EnemyProjectile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "CHPComponent.h"
#include "Missile.h"
#include "CWorld.h"
#include "ChimasLog.h"

Loner::Loner(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), health(nullptr),
    speed(200.0f), direction(1.0f), fireRate(5.0f), fireCooldown(1.0f), width(0.f){}

Loner::~Loner()
{

}

void Loner::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    sprite->LoadTexture("Xenom/ImagesForGame/LonerA.bmp");
    sprite->SetSize(64.0f, 64.0f);

    width = world->GetWorldBounds().x;

    // Add animation component (64x64 tiles)
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(64, 64); // Set tile size after creation

    // Loner animation (first row)
    animation->AddAnimation("move", {
        {0, 0, 0.1f}, {0, 1, 0.1f}, {0, 2, 0.1f}, {0, 3, 0.1f},
        {1, 0, 0.1f}, {1, 1, 0.1f}, {1, 2, 0.1f}, {1, 3, 0.1f},
        {2, 0, 0.1f}, {2, 1, 0.1f}, {2, 2, 0.1f}, {2, 3, 0.1f},
        {3, 0, 0.1f}, {3, 1, 0.1f}, {3, 2, 0.1f}, {3, 3, 0.1f}
        }, true);

    animation->PlayAnimation("move");

    physics = AddComponent<CPhysicsComponent>();

    physics->SetFixedRotation(true);
;
    physics->CreateBoxShape(32.0f, 32.0f, false);
    physics->SetCollisionFilter(
        CollisionCategory::ENEMY,
        CollisionCategory::PLAYER | CollisionCategory::PLAYER_PROJECTILE | CollisionCategory::WALL
    );

    health = AddComponent<CHPComponent>();
    health->SetMaxHP(100.0f);
   

    ChimasLog::Info("Loner spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void Loner::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (physics)
    {
        physics->SetVelocity(Vector2(speed * direction, 0.0f));
    }

    // Fire cooldown and shooting
    fireCooldown -= deltaTime;
    if (fireCooldown <= 0.0f)
    {
        FireProjectile();
        fireCooldown = fireRate;
    }

    if (transform.position.x >= width -30)
    {
        direction = -1.f;

    } else if (transform.position.x <= 30) {

        direction = 1.f;
    }
}
void Loner::FireProjectile()
{
    EnemyProjectile* projectile = world->SpawnActor<EnemyProjectile>();
    if (projectile)
    {
        Vector2 spawnPos = transform.position;
        spawnPos.y += 40.0f;
        projectile->SetPosition(spawnPos);
    }
}
void Loner::OnCollision(CActor* other)
{

    // Always check validity first
    if (IsPendingKill() || !other || other->IsPendingKill())
        return;

    Missile* missile = dynamic_cast<Missile*>(other);
    if (missile)
    {
        ChimasLog::Info("Loner hit by missile!");

        // Reduce health instead of immediate destruction
        health->ChangeHP(-25.0f); // Missile deals 25 damage

        ChimasLog::Info("Loner HP: %.1f / %.1f", health->GetCurrentHP(), health->GetMaxHP());

        // Check if health depleted
        if (health->GetCurrentHP() <= 0.0f)
        {
            ChimasLog::Info("Loner destroyed!");
            Destroy();
        }

        missile->OnCollision(this);
    }
}