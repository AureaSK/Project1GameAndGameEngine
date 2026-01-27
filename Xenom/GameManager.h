#pragma once
#include "CActor.h"
#include <vector>

class CTextWidget;

class GameManager : public CActor
{
private:
    int playerScore;
    int currentWave;
    int enemiesAlive;
    int enemiesPerWave;
	int highScore;
    float waveSpawnDelay;
    float waveTimer;
    bool isGameOver;

    CTextWidget* highScoreText;
    CTextWidget* scoreText;
    CTextWidget* waveText;
    CTextWidget* healthText;

    void SpawnWave();
    void SpawnLoners(int count);
    void SpawnRushers(int count);
    void SpawnDrones(int count);
    void SpawnBigAsteroid(int count);
    void SpawnMediumAsteroid(int count);
    void SpawnSmallAsteroid(int count);

public:
    GameManager(CWorld* world);
    virtual ~GameManager();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    // Score management
    void AddScore(int points);
    int GetScore() const { return playerScore; }

	void SaveHighScore(int score);
	int LoadHighScore();

    void UpdateHealthDisplay(float currentHP, float maxHP);

    // Enemy tracking
    void OnEnemyKilled();
    void OnPlayerDeath();

    // Wave management
    int GetCurrentWave() const { return currentWave; }
    bool IsGameOver() const { return isGameOver; }

    // UI updates
    void UpdateScoreDisplay();
    void UpdateWaveDisplay();
    void ShowGameOverScreen();
};

