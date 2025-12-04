#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;

class EnemyProjectile : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    float speed;
    

public:
    EnemyProjectile(CWorld* world);
    virtual ~EnemyProjectile();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void OnCollision(CActor* other) override;
};