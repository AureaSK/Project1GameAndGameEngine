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
	float direction;


public:
	StoneAsteroidMedium(CWorld* world);
	virtual ~StoneAsteroidMedium();

	void OnCollision(CActor* other);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void SetGameManager(GameManager* manager) { gameManager = manager; }
};

