#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;
class GameManager;

class StoneAsteroidBig : public CActor
{
private:
	CSpriteComponent* sprite;
	CAnimationComponent* animation;
	CPhysicsComponent* physics;
	CHPComponent* health;
	GameManager* gameManager;
	float speed;
	float direction;

	int scoreValue;
	float damage;
	float takenDamage;

public:
	StoneAsteroidBig(CWorld* world);
	virtual ~StoneAsteroidBig();

	float GetDamageValue(float asteroidDamage);

	void OnCollision(CActor* other);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void SetGameManager(GameManager* manager) { gameManager = manager; }
};

