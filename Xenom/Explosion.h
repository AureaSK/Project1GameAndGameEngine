#pragma once
#include "CActor.h"

class CSpriteComponent;
class CAnimationComponent;

class Explosion : public CActor
{
private:
    CSpriteComponent* sprite;
    CAnimationComponent* animation;
    float lifetime;

public:

    Explosion(CWorld* world);
    virtual ~Explosion();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
};

