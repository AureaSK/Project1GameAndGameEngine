#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;


class MetalAsteroidMedium : public CActor
{
private:
	CSpriteComponent* sprite;
	CAnimationComponent* animation;
	CPhysicsComponent* physics;
	float speed;
	float direction;

	float damage;

public:
	MetalAsteroidMedium(CWorld* world);
	virtual ~MetalAsteroidMedium();

	float GetDamageValue(float asteroidDamage);

	void OnCollision(CActor* other);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
};