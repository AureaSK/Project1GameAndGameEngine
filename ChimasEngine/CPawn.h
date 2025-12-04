#pragma once
#include "CActor.h"

class CController;

class CPawn : public CActor
{
protected:
    CController* controller;  // The controller possessing this pawn

public:
    CPawn(CWorld* world);
    virtual ~CPawn();

    // Possession system
    virtual void PossessedBy(CController* newController);
    virtual void UnPossessed();

    // Get the current controller
    CController* GetController() const { return controller; }
    bool IsPossessed() const { return controller != nullptr; }

    // Virtual methods for game-specific input handling
    virtual void SetupPlayerInputComponent() {}

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
};

