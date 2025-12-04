#include "pch.h"
#include "CController.h"
#include "CPawn.h"

CController::CController(CWorld* world) : CActor(world), possessedPawn(nullptr){}

CController::~CController()
{
	UnPossess();
}

void CController::Possess(CPawn* pawn)
{
    if (possessedPawn)
    {
        UnPossess();
    }

    possessedPawn = pawn;

    if (possessedPawn)
    {
        possessedPawn->PossessedBy(this);
    }
}

void CController::UnPossess()
{
    if (possessedPawn)
    {
        possessedPawn->UnPossessed();
        possessedPawn = nullptr;
    }
}

void CController::BeginPlay()
{
    CActor::BeginPlay();
}

void CController::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);
}
