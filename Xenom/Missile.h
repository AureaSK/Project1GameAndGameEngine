#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;

class Missile : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    float speed;

public:
    Missile(CWorld* world);
    virtual ~Missile();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void OnCollision(CActor* other) override;
};