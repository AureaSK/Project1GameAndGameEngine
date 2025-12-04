#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;

class Loner : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    float speed;
    float direction;
    float fireRate;
    float fireCooldown;

    float width;

    void FireProjectile();

    void OnCollision(CActor* other);

public:
    Loner(CWorld* world);
    virtual ~Loner();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    void SetDirection(float dir) { direction = dir; }
};

