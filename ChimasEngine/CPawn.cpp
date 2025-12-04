#include "pch.h"
#include "CPawn.h"

CPawn::CPawn(CWorld* world) : CActor(world), controller(nullptr){}

CPawn::~CPawn()
{
	if (controller)
	{
		UnPossessed();
	}
}

void CPawn::PossessedBy(CController* newController)
{
    if (controller)
    {
        UnPossessed();
    }

    controller = newController;

    if (controller)
    {
        SetupPlayerInputComponent();
    }
}

void CPawn::UnPossessed()
{
    if (controller)
    {
        controller = nullptr;
    }
}

void CPawn::BeginPlay()
{
    CActor::BeginPlay();
}

void CPawn::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);
}


