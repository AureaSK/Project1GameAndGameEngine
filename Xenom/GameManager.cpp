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
#include "MetalAsteroidBig.h"
#include "MetalAsteroidMedium.h"
#include "MetalAsteroidSmall.h"
#include "ChimasLog.h"
#include <string>
#include <random>
#include <fstream>

GameManager::GameManager(CWorld* world)
    : CActor(world), playerScore(0), currentWave(0), enemiesAlive(0),
    enemiesPerWave(5), waveSpawnDelay(3.0f), waveTimer(0.0f),
	isGameOver(false), scoreText(nullptr), waveText(nullptr), 
    healthText(nullptr), highScoreText(nullptr)
{
}

GameManager::~GameManager()
{
    // HUD owns the widgets, so we don't delete them here
}

void GameManager::BeginPlay()
{
    CActor::BeginPlay();
	highScore = LoadHighScore();

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

        // Create score display (top-middle)
        highScoreText = hud->CreateWidget<CTextWidget>();
        highScoreText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
        highScoreText->SetText("HIGH SCORE\n\n   " + std::to_string(highScore));
        highScoreText->SetPosition(Vector2(325.0f, 10.0f));
        highScoreText->SetSize(Vector2(300.0f, 30.0f));
        highScoreText->SetScale(0.75f);
        highScoreText->SetHorizontalAlignment(CTextWidget::TextAlign::Left);

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
    int dronesToSpawn = (currentWave >= 2) ? (currentWave / 3) : 0;

	// Asteroids (Stone)
	int bigAsteroidsToSpawn = (currentWave >= 7) ? 2 : (currentWave >= 3) ? 1 : 0;

    int mediumAsteroidsToSpawn = (currentWave >= 5) ? 2 : (currentWave >= 2) ? 1 : 0;
    
    int smallAsteroidsToSpawn = 0;
    switch (currentWave) {
    case 1: smallAsteroidsToSpawn = 2; break;
    case 2: smallAsteroidsToSpawn = 3; break;
    default: smallAsteroidsToSpawn = 2; break;
    }
    // Asteroids (Metal)
    int bigMetalAsteroidsToSpawn = (currentWave >= 7) ? 2 : (currentWave >= 3) ? 1 : 0;

    int mediumMetalAsteroidsToSpawn = (currentWave >= 5) ? 2 : (currentWave >= 2) ? 1 : 0;

    int smallMetalAsteroidsToSpawn = 0;
    switch (currentWave) {
    case 1: smallMetalAsteroidsToSpawn = 2; break;
    case 2: smallMetalAsteroidsToSpawn = 3; break;
    default: smallMetalAsteroidsToSpawn = 2; break;
    }

    SpawnLoners(lonersToSpawn);
    SpawnRushers(rushersToSpawn);
    SpawnDrones(dronesToSpawn);
	SpawnBigAsteroid(bigAsteroidsToSpawn);
	SpawnMediumAsteroid(mediumAsteroidsToSpawn);    
	SpawnSmallAsteroid(smallAsteroidsToSpawn);
    SpawnBigMetalAsteroid(bigMetalAsteroidsToSpawn);
    SpawnMediumMetalAsteroid(mediumMetalAsteroidsToSpawn);
    SpawnSmallMetalAsteroid(smallMetalAsteroidsToSpawn);

    enemiesAlive = lonersToSpawn + rushersToSpawn + (dronesToSpawn*3);

    UpdateWaveDisplay();

    ChimasLog::Info("Wave %d spawned! Enemies: %d", currentWave, enemiesAlive);
}

void GameManager::SpawnLoners(int count)
{
	float direction = 1.0f;

    for (int i = 0; i < count; i++)
    {
        Loner* loner = world->SpawnActor<Loner>();
        if (loner)
        {
            std::random_device rd;                  // seed
            std::mt19937 gen(rd());                 // Mersenne Twister engine
            std::uniform_int_distribution<> dist(100, 800);

            float yPos = 100.0f + (i % 4) * 75.0f;
            loner->SetPosition(Vector2(dist(gen), yPos));
            
            loner->SetDirection(direction);
			direction *= -1.0f; // Alternate direction for next loner

            loner->SetGameManager(this); // Set reference to game manager
        }
    }
}

void GameManager::SpawnRushers(int count)
{

    for (int i = 0; i < count; i++)
    {
        Rusher* rusher = world->SpawnActor<Rusher>();
        if (rusher)
        {
            std::random_device rd;                  
            std::mt19937 gen(rd());                 
            std::uniform_int_distribution<> distX(100, 700);           
            std::uniform_int_distribution<> distY(100, 500);

            rusher->SetPosition(Vector2(distX(gen), -distY(gen)));
            rusher->SetGameManager(this); // Set reference to game manager
        }
    }
}

void GameManager::SpawnDrones(int count)
{
    
    for (int a = 0; a < count; a++)
    {
        std::random_device rd;                  
        std::mt19937 gen(rd());                 
        std::uniform_int_distribution<> distX(200, 600);
        std::uniform_int_distribution<> distY(100, 500);

		float randomY = distY(gen);
        float randomX = distX(gen);

        for (int i = 0; i < 3; i++)
        {
            Drone* drone = world->SpawnActor<Drone>();
            if (drone)
            {
                float yOffset = i * 50.0f;
                drone->SetBaseX(randomX); // Center X for sine wave
                drone->SetPosition(Vector2(0.0f, -randomY + yOffset));
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
            std::random_device rd;                  
            std::mt19937 gen(rd());                 
            std::uniform_int_distribution<> distX(200, 600);
            std::uniform_int_distribution<> distY(100, 600);

			bigAsteroid->SetPosition(Vector2(distX(gen), -distY(gen)));
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
            std::random_device rd;                  
            std::mt19937 gen(rd());                 
            std::uniform_int_distribution<> distX(150, 650);
            std::uniform_int_distribution<> distY(100, 600);

            mediumAsteroid->SetPosition(Vector2(distX(gen), -distY(gen)));
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
            std::random_device rd;                  
            std::mt19937 gen(rd());                
            std::uniform_int_distribution<> distX(100, 700);
            std::uniform_int_distribution<> distY(100, 600);

            smallAsteroid->SetPosition(Vector2(distX(gen), -distY(gen)));
            smallAsteroid->SetGameManager(this); // Set reference to game manager

        }

    }
}

void GameManager::SpawnBigMetalAsteroid(int count)
{
    for (int i = 0; i < count; i++)
    {
        MetalAsteroidBig* bigMetalAsteroid = world->SpawnActor<MetalAsteroidBig>();

        if (bigMetalAsteroid)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distX(200, 600);
            std::uniform_int_distribution<> distY(100, 600);

            bigMetalAsteroid->SetPosition(Vector2(distX(gen), -distY(gen)));

        }

    }
}

void GameManager::SpawnMediumMetalAsteroid(int count)
{
    for (int i = 0; i < count; i++)
    {
        MetalAsteroidMedium* mediumMetalAsteroid = world->SpawnActor<MetalAsteroidMedium>();

        if (mediumMetalAsteroid)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distX(150, 650);
            std::uniform_int_distribution<> distY(100, 600);

            mediumMetalAsteroid->SetPosition(Vector2(distX(gen), -distY(gen)));

        }

    }
}

void GameManager::SpawnSmallMetalAsteroid(int count)
{
    for (int i = 0; i < count; i++)
    {
        MetalAsteroidSmall* smallMetalAsteroid = world->SpawnActor<MetalAsteroidSmall>();

        if (smallMetalAsteroid)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distX(100, 700);
            std::uniform_int_distribution<> distY(100, 600);

            smallMetalAsteroid->SetPosition(Vector2(distX(gen), -distY(gen)));
            
        }

    }
}


void GameManager::AddScore(int points)
{
    playerScore += points;
    UpdateScoreDisplay();
    ChimasLog::Info("Score: %d (+%d)", playerScore, points);
}

// Save high score
void GameManager::SaveHighScore(int score)
{
    std::ofstream file("highscore.txt");
    if (file.is_open())
    {
        file << score;
        file.close();
    }
}

// Load high score
int GameManager::LoadHighScore()
{
    std::ifstream file("highscore.txt");
    int score = 0;
    if (file.is_open())
    {
        file >> score;
        file.close();
    }
    else
    {
        // File doesn't exist yet, create it with default value
        SaveHighScore(0);
    }
    return score;
}

void GameManager::OnEnemyKilled()
{
    enemiesAlive--;

    if (enemiesAlive <= 0)
    {
        ChimasLog::Info("Wave %d cleared!", currentWave);
    }
}

void GameManager::OnPlayerDeath()
{
    isGameOver = true;
	// Update High Score to new Score if better
	if (playerScore > highScore)
	{
		highScore = playerScore;
		SaveHighScore(highScore);
	}
    ShowGameOverScreen();
    ChimasLog::Info("GAME OVER! Final Score: %d, Waves Completed: %d", playerScore, currentWave);
}

void GameManager::UpdateScoreDisplay()
{
    if (scoreText)
    {
        scoreText->SetText("SCORE\n\n" + std::to_string(playerScore));
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

    // High score
    CTextWidget* highScoreText = hud->CreateWidget<CTextWidget>();
    highScoreText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
    highScoreText->SetText("HIGH SCORE: " + std::to_string(highScore));
    highScoreText->SetPosition(Vector2(0.0f, 380.0f));
    highScoreText->SetSize(Vector2(800.0f, 40.0f));
    highScoreText->SetScale(1.5f);
    highScoreText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);
    highScoreText->SetZOrder(100);

    // Final score
    CTextWidget* finalScoreText = hud->CreateWidget<CTextWidget>();
    finalScoreText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
    finalScoreText->SetText("FINAL SCORE: " + std::to_string(playerScore));
    finalScoreText->SetPosition(Vector2(0.0f, 430.0f));
    finalScoreText->SetSize(Vector2(800.0f, 40.0f));
    finalScoreText->SetScale(1.5f);
    finalScoreText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);
    finalScoreText->SetZOrder(100);

    // Waves survived
    CTextWidget* wavesText = hud->CreateWidget<CTextWidget>();
    wavesText->LoadGridFont(fontPath, 16, 16, " !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");
    wavesText->SetText("WAVES SURVIVED: " + std::to_string(--currentWave));
    wavesText->SetPosition(Vector2(0.0f, 480.0f));
    wavesText->SetSize(Vector2(800.0f, 40.0f));
    wavesText->SetScale(1.5f);
    wavesText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);
    wavesText->SetZOrder(100);
}