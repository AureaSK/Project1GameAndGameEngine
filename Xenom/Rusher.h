#pragma once

#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;

class Rusher : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    CHPComponent* health;

    float speed;
    float direction;
    float damage;
    float takenDamage;

public:
    Rusher(CWorld* world);
    virtual ~Rusher();

    void OnCollision(CActor* other);

    float GetDamageValue(float enemyDamage);

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
};

