#include "pch.h"
#include "Loner.h"
#include "EnemyProjectile.h"
#include "CSpriteComponent.h"
#include "CAnimationComponent.h"
#include "CPhysicsComponent.h"
#include "CHPComponent.h"
#include "Missile.h"
#include "SpaceshipPawn.h"
#include "Explosion.h"
#include "CWorld.h"
#include "ChimasLog.h"
#include "CHUD.h"
#include "CFloatingTextWidget.h"
#include "CEngine.h"

Loner::Loner(CWorld* world)
    : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), health(nullptr),
    speed(200.0f), direction(1.0f), fireRate(5.0f), fireCooldown(1.0f), width(0.f), takenDamage(0.f){}

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

        takenDamage = missile->GetDamageValue(takenDamage);

        // Reduce health instead of immediate destruction
        health->ChangeHP(-takenDamage); // Missile deals 25 damage

        ChimasLog::Info("Loner HP: %.1f / %.1f", health->GetCurrentHP(), health->GetMaxHP());

        // Check if health depleted
        if (health->GetCurrentHP() <= 0.0f)
        {
            ChimasLog::Info("Loner destroyed!");

            // Create floating text on death
            CHUD* hud = world->GetHUD();
            if (hud)
            {
                CFloatingTextWidget* floatingText = hud->CreateWidget<CFloatingTextWidget>();

                // Load font
                std::string fontPath = world->GetEngine()->ResolveAssetPath("Xenom/ImagesForGame/font16x16.bmp");
                floatingText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");

                // Convert game world position to screen position
                // Game is rotated -90 degrees, so we need to transform:
                // Original: (gameX, gameY) -> Screen: (gameY, screenHeight - gameX)
                Vector2 gamePos = transform.position;
                Vector2 screenBounds = world->GetWorldBounds();

                // Transform rotated game coordinates to screen coordinates
                Vector2 screenPos;
                screenPos.x = screenBounds.x - gamePos.y;  // Game Y becomes screen X
                screenPos.y = gamePos.x;  // Game X becomes inverted screen Y

                // Configure the floating text
                floatingText->SetText("DESTROYED!");
                floatingText->SetTarget(nullptr);  // Don't follow the actor since it's about to be destroyed
                floatingText->SetLifetime(2.0f);
                floatingText->SetFloatSpeed(10.0f);
                floatingText->SetScale(.5f);

                // Set position and size
                floatingText->SetPosition(screenPos);
                floatingText->SetSize(Vector2(200.0f, 50.0f));
                floatingText->SetHorizontalAlignment(CFloatingTextWidget::TextAlign::Center);

                ChimasLog::Info("Floating text created for destroyed Loner");
            }

            Destroy();
        }

        missile->OnCollision(this);
    }

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Rusher hit spaceship!");
        Destroy();

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }

        // DEAL DAMAGE TO PLAYER
    }
}