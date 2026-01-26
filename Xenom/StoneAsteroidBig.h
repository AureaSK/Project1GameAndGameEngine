#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;



class StoneAsteroidBig : public CActor
{
private:
	CSpriteComponent* sprite;
	CAnimationComponent* animation;
	CPhysicsComponent* physics;
	CHPComponent* health;
	float speed;
	float direction;


public:
	StoneAsteroidBig(CWorld* world);
	virtual ~StoneAsteroidBig();

	void OnCollision(CActor* other);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
};

