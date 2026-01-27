#include "StoneAsteroidSmall.h"
#include "pch.h"
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
#include "GameManager.h"

StoneAsteroidSmall::StoneAsteroidSmall(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr), physics(nullptr), speed(100.0f), direction(0, 1), damage(10.f), scoreValue(10) {}

StoneAsteroidSmall::~StoneAsteroidSmall()
{

}

void StoneAsteroidSmall::BeginPlay()
{
    CActor::BeginPlay();

    // Add sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (sprite->LoadTexture("Xenom/ImagesForGame/SAster32.bmp"))
    {
        sprite->SetSize(32.0f, 32.0f);
    }

    // Add animation component 
    animation = AddComponent<CAnimationComponent>();
    animation->SetTileSize(32, 32);

    // Small Asteroid animation
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
        CollisionCategory::PLAYER | CollisionCategory::PLAYER_PROJECTILE
    );

    // FIXED: Create as SOLID collision (sensor = false for physical collision)
    physics->CreateBoxShape(30.0f, 30.0f, false);

    health = AddComponent<CHPComponent>();
    health->SetMaxHP(10.f);

    ChimasLog::Info("Small Stone Asteroid spawned at (%.1f, %.1f)", transform.position.x, transform.position.y);
}

void StoneAsteroidSmall::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (physics)
    {
        physics->SetVelocity(Vector2(speed * direction.x, speed * direction.y));
    }


    // Destroy if off screen
    if (transform.position.y > (world->GetWorldBounds().y + 150))
    {
        Destroy(); 
    }
}

void StoneAsteroidSmall::OnCollision(CActor* other)
{
    if (IsPendingKill() || !other || other->IsPendingKill()) return;

    Missile* missile = dynamic_cast<Missile*>(other);
    if (missile)
    {
        ChimasLog::Info("Small Stone Asteroid hit by missile!");

        takenDamage = missile->GetDamageValue(takenDamage);

        // Reduce health instead of immediate destruction
        health->ChangeHP(-takenDamage); // Missile deals 25 damage

        ChimasLog::Info("Small Stone Asteroid HP: %.1f / %.1f", health->GetCurrentHP(), health->GetMaxHP());


        // Check if health depleted
        if (health->GetCurrentHP() <= 0.0f)
        {
            ChimasLog::Info("Small Stone Asteroid destroyed!");

            if (gameManager)
            {
                gameManager->AddScore(scoreValue);
            }

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

                floatingText->SetText("+" + std::to_string(scoreValue));
                floatingText->SetTarget(nullptr);
                floatingText->SetLifetime(1.5f);
                floatingText->SetFloatSpeed(30.0f);
                floatingText->SetScale(1.0f);
                floatingText->SetPosition(screenPos);
                floatingText->SetSize(Vector2(100.0f, 30.0f));
                floatingText->SetHorizontalAlignment(CFloatingTextWidget::TextAlign::Center);

                ChimasLog::Info("Floating text created for destroyed Asteroid");
            }

            Destroy();
        }

        missile->OnCollision(this);
    }

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Small Asteroid hit spaceship!");
        Destroy();

        Explosion* kabum = world->SpawnActor<Explosion>();
        if (kabum)
        {
            kabum->SetPosition(transform.position);
        }
    }
}

float StoneAsteroidSmall::GetDamageValue(float asteroidDamage)
{
    return damage;
}