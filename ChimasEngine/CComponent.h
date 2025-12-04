#pragma once

class CActor;

class CComponent
{
protected:
    CActor* owner;
    bool isActive;

public:
    CComponent(CActor* owner) : owner(owner), isActive(true) {}
    virtual ~CComponent() {}

    // Lifecycle
    virtual void BeginPlay() {}
    virtual void Tick(float deltaTime) {}
    virtual void Destroy() {}

    // State
    bool IsActive() const { return isActive; }
    void SetActive(bool active) { isActive = active; }

    // Owner access
    CActor* GetOwner() const { return owner; }
};