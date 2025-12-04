#pragma once

#include "CController.h" 

class PlayerController : public CController
{
public:
    PlayerController(CWorld* world);
    virtual ~PlayerController();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
};

