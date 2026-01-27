#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;
class GameManager;


class StoneAsteroidMedium : public CActor
{
private:
	CSpriteComponent* sprite;
	CAnimationComponent* animation;
	CPhysicsComponent* physics;
	CHPComponent* health;
	GameManager* gameManager;
	float speed;
	Vector2 direction;

	int scoreValue;
	float damage;
	float takenDamage;

public:
	StoneAsteroidMedium(CWorld* world);
	virtual ~StoneAsteroidMedium();

	float GetDamageValue(float asteroidDamage);

	void OnCollision(CActor* other);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void SetGameManager(GameManager* manager) { gameManager = manager; }
	void SetDirection(Vector2 dir) { direction = dir; }
};

