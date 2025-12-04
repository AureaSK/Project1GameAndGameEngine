#pragma once

#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;

class Rusher : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    float speed;
    float direction;

public:
    Rusher(CWorld* world);
    virtual ~Rusher();

    void OnCollision(CActor* other);

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
};

