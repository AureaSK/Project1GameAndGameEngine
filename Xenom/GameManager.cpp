#include "pch.h"
#include "GameManager.h"
#include "CWorld.h"
#include "CHUD.h"
#include "CTextWidget.h"
#include "CEngine.h"
#include "Loner.h"
#include "Rusher.h"
#include "Drone.h"
#include "StoneAsteroidBig.h"
#include "StoneAsteroidMedium.h"
#include "StoneAsteroidSmall.h"
#include "ChimasLog.h"
#include <string>
#include <random>

GameManager::GameManager(CWorld* world)
    : CActor(world), playerScore(0), currentWave(0), enemiesAlive(0),
    enemiesPerWave(5), waveSpawnDelay(3.0f), waveTimer(0.0f),
    isGameOver(false), scoreText(nullptr), waveText(nullptr)
{
}

GameManager::~GameManager()
{
    // HUD owns the widgets, so we don't delete them here
}

void GameManager::BeginPlay()
{
    CActor::BeginPlay();

    CHUD* hud = world->GetHUD();
    if (hud)
    {
        std::string fontPath = world->GetEngine()->ResolveAssetPath("Xenom/ImagesForGame/font16x16.bmp");

        // Create score display (top-left)
        scoreText = hud->CreateWidget<CTextWidget>();
        scoreText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
        scoreText->SetPosition(Vector2(10.0f, 10.0f));
        scoreText->SetSize(Vector2(300.0f, 30.0f));
        scoreText->SetScale(1.0f);
        scoreText->SetHorizontalAlignment(CTextWidget::TextAlign::Left);

        // Create wave display (top-right)
        waveText = hud->CreateWidget<CTextWidget>();
        waveText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
        waveText->SetPosition(Vector2(475.0f, 10.0f));
        waveText->SetSize(Vector2(300.0f, 30.0f));
        waveText->SetScale(1.0f);
        waveText->SetHorizontalAlignment(CTextWidget::TextAlign::Right);

        // CREATE HEALTH DISPLAY (BOTTOM-LEFT) - ADD THIS:
        healthText = hud->CreateWidget<CTextWidget>();
        healthText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
        healthText->SetPosition(Vector2(10.0f, 775.f)); // Bottom-left (adjust Y based on your screen height)
        healthText->SetSize(Vector2(300.0f, 30.0f));
        healthText->SetScale(1.0f);
        healthText->SetHorizontalAlignment(CTextWidget::TextAlign::Left);

        UpdateScoreDisplay();
        UpdateWaveDisplay();
    }

    // Start first wave after delay
    waveTimer = 2.0f; // 2 second delay before first wave

    ChimasLog::Info("GameManager initialized");
}

void GameManager::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    if (isGameOver) return;

    // Check if wave should spawn
    if (enemiesAlive == 0)
    {
        waveTimer -= deltaTime;

        if (waveTimer <= 0.0f)
        {
            SpawnWave();
            waveTimer = waveSpawnDelay; // Reset for next wave
        }
    }
}

void GameManager::SpawnWave()
{
    currentWave++;

    // Scale difficulty with waves
    int lonersToSpawn = 2 + (currentWave / 2);
    int rushersToSpawn = 1 + (currentWave / 3);
    int dronesToSpawn = (currentWave > 2) ? (currentWave / 2) : 0;
	int bigAsteroidsToSpawn = (currentWave >= 5) ? 1 : 0;
	int mediumAsteroidsToSpawn = (currentWave >= 3) ? 1 : 0;
	int smallAsteroidsToSpawn = currentWave;

    SpawnLoners(lonersToSpawn);
    SpawnRushers(rushersToSpawn);
    SpawnDrones(dronesToSpawn);
	SpawnBigAsteroid(bigAsteroidsToSpawn);
	SpawnMediumAsteroid(mediumAsteroidsToSpawn);    
	SpawnSmallAsteroid(smallAsteroidsToSpawn);

    enemiesAlive = lonersToSpawn + rushersToSpawn + (dronesToSpawn*3);

    UpdateWaveDisplay();

    ChimasLog::Info("Wave %d spawned! Enemies: %d", currentWave, enemiesAlive);
}

void GameManager::SpawnLoners(int count)
{
    float screenWidth = world->GetWorldBounds().x;

    for (int i = 0; i < count; i++)
    {
        Loner* loner = world->SpawnActor<Loner>();
        if (loner)
        {
            float xPos = 100.0f + (i * (screenWidth - 200.0f) / count);
            float yPos = 100.0f + (i % 2) * 50.0f;
            loner->SetPosition(Vector2(xPos, yPos));
            loner->SetDirection(1.0f);
            loner->SetGameManager(this); // Set reference to game manager
        }
    }
}

void GameManager::SpawnRushers(int count)
{
    float screenWidth = world->GetWorldBounds().x;

    for (int i = 0; i < count; i++)
    {
        Rusher* rusher = world->SpawnActor<Rusher>();
        if (rusher)
        {
            std::random_device rd;                  // seed
            std::mt19937 gen(rd());                 // Mersenne Twister engine
            std::uniform_int_distribution<> dist(100, 700);

            float yPos = 100.0f;
            rusher->SetPosition(Vector2(dist(gen), yPos));
            rusher->SetGameManager(this); // Set reference to game manager
        }
    }
}

void GameManager::SpawnDrones(int count)
{
    
    for (int a = 0; a < count; a++)
    {
        std::random_device rd;                  // seed
        std::mt19937 gen(rd());                 // Mersenne Twister engine
        std::uniform_int_distribution<> dist(200, 600);

        float randomX = dist(gen);

        for (int i = 0; i < 3; i++)
        {
            Drone* drone = world->SpawnActor<Drone>();
            if (drone)
            {
                float yOffset = i * 50.0f;
                drone->SetBaseX(randomX); // Center X for sine wave
                drone->SetPosition(Vector2(0.0f, 100.0f + yOffset));
                drone->SetTimeOffset(i * 0.1f);
                drone->SetGameManager(this); // Set reference to game manager
            }
        }
    }
    
}

void GameManager::SpawnBigAsteroid(int count)
{
    for (int i = 0; i < count; i++)
    {
        StoneAsteroidBig* bigAsteroid = world->SpawnActor<StoneAsteroidBig>();

        if (bigAsteroid)
        {
            std::random_device rd;                  // seed
            std::mt19937 gen(rd());                 // Mersenne Twister engine
            std::uniform_int_distribution<> dist(200, 600);

			bigAsteroid->SetPosition(Vector2(dist(gen), 100.0f));
			bigAsteroid->SetGameManager(this); // Set reference to game manager

        }

    }
}

void GameManager::SpawnMediumAsteroid(int count)
{
    for (int i = 0; i < count; i++)
    {
        StoneAsteroidMedium* mediumAsteroid = world->SpawnActor<StoneAsteroidMedium>();

        if (mediumAsteroid)
        {
            std::random_device rd;                  // seed
            std::mt19937 gen(rd());                 // Mersenne Twister engine
            std::uniform_int_distribution<> dist(150, 650);

            mediumAsteroid->SetPosition(Vector2(dist(gen), 100.0f));
            mediumAsteroid->SetGameManager(this); // Set reference to game manager

        }

    }
}

void GameManager::SpawnSmallAsteroid(int count)
{
    for (int i = 0; i < count; i++)
    {
        StoneAsteroidSmall* smallAsteroid = world->SpawnActor<StoneAsteroidSmall>();

        if (smallAsteroid)
        {
            std::random_device rd;                  // seed
            std::mt19937 gen(rd());                 // Mersenne Twister engine
            std::uniform_int_distribution<> dist(100, 700);
            
            smallAsteroid->SetPosition(Vector2(dist(gen), 100.0f));
            smallAsteroid->SetGameManager(this); // Set reference to game manager

        }

    }
}

void GameManager::AddScore(int points)
{
    playerScore += points;
    UpdateScoreDisplay();
    ChimasLog::Info("Score: %d (+%d)", playerScore, points);
}

void GameManager::OnEnemyKilled()
{
    enemiesAlive--;
    ChimasLog::Info("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEnemy killed! Remaining: %d", enemiesAlive);

    if (enemiesAlive <= 0)
    {
        ChimasLog::Info("Wave %d cleared!", currentWave);
    }
}

void GameManager::OnPlayerDeath()
{
    isGameOver = true;
    ShowGameOverScreen();
    ChimasLog::Info("GAME OVER! Final Score: %d, Waves Completed: %d", playerScore, currentWave);
}

void GameManager::UpdateScoreDisplay()
{
    if (scoreText)
    {
        scoreText->SetText("SCORE: " + std::to_string(playerScore));
    }
}

void GameManager::UpdateWaveDisplay()
{
    if (waveText)
    {
        waveText->SetText("WAVE: " + std::to_string(currentWave));
    }
}

void GameManager::UpdateHealthDisplay(float currentHP, float maxHP)
{
    if (healthText)
    {
        int hpInt = static_cast<int>(currentHP);
        int maxHPInt = static_cast<int>(maxHP);
        healthText->SetText("HP: " + std::to_string(hpInt) + " / " + std::to_string(maxHPInt));
    }
}

void GameManager::ShowGameOverScreen()
{
    CHUD* hud = world->GetHUD();
    if (!hud) return;

    std::string fontPath = world->GetEngine()->ResolveAssetPath("Xenom/ImagesForGame/font16x16.bmp");

    // Game Over title
    CTextWidget* gameOverText = hud->CreateWidget<CTextWidget>();
    gameOverText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
    gameOverText->SetText("GAME OVER");
    gameOverText->SetPosition(Vector2(0.0f, 300.0f));
    gameOverText->SetSize(Vector2(800.0f, 60.0f));
    gameOverText->SetScale(3.0f);
    gameOverText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);
    gameOverText->SetZOrder(100); // Draw on top

    // Final score
    CTextWidget* finalScoreText = hud->CreateWidget<CTextWidget>();
    finalScoreText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
    finalScoreText->SetText("FINAL SCORE: " + std::to_string(playerScore));
    finalScoreText->SetPosition(Vector2(0.0f, 380.0f));
    finalScoreText->SetSize(Vector2(800.0f, 40.0f));
    finalScoreText->SetScale(1.5f);
    finalScoreText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);
    finalScoreText->SetZOrder(100);

    // Waves survived
    CTextWidget* wavesText = hud->CreateWidget<CTextWidget>();
    wavesText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
    wavesText->SetText("WAVES SURVIVED: " + std::to_string(currentWave));
    wavesText->SetPosition(Vector2(0.0f, 430.0f));
    wavesText->SetSize(Vector2(800.0f, 40.0f));
    wavesText->SetScale(1.5f);
    wavesText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);
    wavesText->SetZOrder(100);
}