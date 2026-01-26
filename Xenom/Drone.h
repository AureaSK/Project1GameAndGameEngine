#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;
class GameManager;


class Drone : public CActor
{
private:
	CSpriteComponent* sprite;
	CAnimationComponent* animation;
	CPhysicsComponent* physics;
	CHPComponent* health;
	GameManager* gameManager;

	float speed;
	float direction;
	float timeAccumulator = 0.0f;
	float timeOffset = 0.0f;       // Time delay for this drone
    float sineFrequency = 3.0f;    // How fast it oscillates
    float sineAmplitude = 100.0f;   // How far it moves side to side
	float baseX = 200.0f;		   // Center X position for the wave
	int scoreValue;

	float damage;
	float takenDamage;
	
public:
	Drone(CWorld* world);
	virtual ~Drone();

	void OnCollision(CActor* other);

	float GetDamageValue(float enemyDamage);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void SetGameManager(GameManager* manager) { gameManager = manager; }
	void SetTimeOffset(float offset) { timeOffset = offset; }
	void SetBaseX(float x) { baseX = x; }
};

