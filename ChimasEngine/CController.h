#pragma once
#include "CActor.h"

class CPawn;

class CController : public CActor
{
protected:
    CPawn* possessedPawn;

public:
    CController(CWorld* world);
    virtual ~CController();

    // Possession methods
    virtual void Possess(CPawn* pawn);
    virtual void UnPossess();

    // Get the currently possessed pawn
    CPawn* GetPawn() const { return possessedPawn; }
    bool HasPawn() const { return possessedPawn != nullptr; }

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
};

