#pragma once

#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;
class CPhysicsComponent;
class CHPComponent;
class GameManager;

class Rusher : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    CPhysicsComponent* physics;
    CHPComponent* health;
    GameManager* gameManager;

    float speed;
    float direction;
    float damage;
    float takenDamage;
    int scoreValue;

public:
    Rusher(CWorld* world);
    virtual ~Rusher();

    void OnCollision(CActor* other);

    float GetDamageValue(float enemyDamage);

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    void SetGameManager(GameManager* manager) { gameManager = manager; }
};

