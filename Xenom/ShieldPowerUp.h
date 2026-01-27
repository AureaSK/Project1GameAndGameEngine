#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class GameManager;


class ShieldPowerUp : public CActor
{
private:
	CSpriteComponent* sprite;
	CAnimationComponent* animation;
	CPhysicsComponent* physics;
	
	float speed;
	float direction;

	float heal;

public:
	ShieldPowerUp(CWorld* world);
	virtual ~ShieldPowerUp();

	float GetDamageValue(float powerUpHeal);

	void OnCollision(CActor* other);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
};
