#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;
class GameManager;

class Loner : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    CHPComponent* health;
    GameManager* gameManager;

    float speed;
    float direction;
    float fireRate;
    float fireCooldown;
    float takenDamage;
    float width;
    float damage;
    int scoreValue;

    void FireProjectile();
    void OnCollision(CActor* other);

public:
    Loner(CWorld* world);
    virtual ~Loner();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    float GetDamageValue(float enemyDamage);
    void SetDirection(float dir) { direction = dir; }
    void SetGameManager(GameManager* manager) { gameManager = manager; }
};