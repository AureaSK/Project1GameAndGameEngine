#pragma once

#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;

class Spaceship : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;

    float moveSpeed;
    float fireRate;
    float fireCooldown;

public:

    Spaceship(CWorld* world);
    virtual ~Spaceship();
    
    void OnCollision(CActor* other);

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void Destroy() override;  // Override to trigger game over

    void FireMissile();
};