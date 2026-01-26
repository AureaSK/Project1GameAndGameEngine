#include "pch.h"
#include "Drone.h"
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

Drone::Drone(CWorld* world) : CActor(world), sprite(nullptr), animation(nullptr),
physics(nullptr), health(nullptr), gameManager(nullptr), speed(100.0f), direction(1.f), damage(25.f), scoreValue(50) {}

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

    health = AddComponent<CHPComponent>();
    health->SetMaxHP(50.f);

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
        ChimasLog::Info("Loner hit by missile!");

        // Notify game manager
        if (gameManager)
        {
            gameManager->AddScore(scoreValue);
            gameManager->OnEnemyKilled();
        }

        takenDamage = missile->GetDamageValue(takenDamage);

        // Reduce health instead of immediate destruction
        health->ChangeHP(-takenDamage); // Missile deals 25 damage

        ChimasLog::Info("Loner HP: %.1f / %.1f", health->GetCurrentHP(), health->GetMaxHP());

        // Check if health depleted
        if (health->GetCurrentHP() <= 0.0f)
        {
            ChimasLog::Info("Drone destroyed!");

            if (gameManager)
            {
                gameManager->AddScore(scoreValue);
                gameManager->OnEnemyKilled();
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

                ChimasLog::Info("Floating text created for destroyed Drone");
            }

            Destroy();
        }

        missile->OnCollision(this);
    }

    SpaceshipPawn* Spaceship = dynamic_cast<SpaceshipPawn*>(other);
    if (Spaceship)
    {
        ChimasLog::Info("Drone hit spaceship!");

        Explosion* explosion = world->SpawnActor<Explosion>();
        if (explosion)
        {
            explosion->SetPosition(transform.position);
        }

        // Notify game manager
        if (gameManager)
        {
            gameManager->OnEnemyKilled();
        }

        Destroy();
    }

}
float Drone::GetDamageValue(float enemyDamage)
{
    return damage;
}