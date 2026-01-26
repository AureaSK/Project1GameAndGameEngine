#pragma once

#include "CPawn.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;
class GameManager;

class SpaceshipPawn : public CPawn
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    CHPComponent* health;
    GameManager* gameManager;


    float moveSpeed;
    float fireRate;
    float fireCooldown;
    float takenDamage;

public:
    SpaceshipPawn(CWorld* world);
    virtual ~SpaceshipPawn();

    void OnCollision(CActor* other);

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void Destroy() override;

    // Methods called BY the controller
    void MoveInDirection(const Vector2& direction);
    void FireWeapon();

    void SetGameManager(GameManager* manager) { gameManager = manager; }
};

