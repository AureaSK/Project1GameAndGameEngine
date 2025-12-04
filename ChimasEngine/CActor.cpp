#include "pch.h"
#include "CActor.h"
#include "CComponent.h"
#include "SDL3/SDL.h"

CActor::CActor(CWorld* world) : world(world), isPendingKill(false), isActive(true) {}

CActor::~CActor()
{
    for (CComponent* comp : components)
    {
        delete comp;
    }
    components.clear();
}
void CActor::BeginPlay()
{
    // Override in derived classes for initialization
}
void CActor::Tick(float deltaTime)
{
    // Update all components
    for (CComponent* comp : components)
    {
        if (comp && comp->IsActive())
        {
            comp->Tick(deltaTime);
        }
    }
}
void CActor::Destroy()
{
    isPendingKill = true;

   

    // Destroy all components
    for (CComponent* comp : components)
    {
        if (comp)
        {
            comp->Destroy();
        }
    }
}