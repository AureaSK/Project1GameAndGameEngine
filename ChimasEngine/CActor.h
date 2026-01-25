#pragma once

#include <vector>
#include "ChimasUtilities.h"

class CWorld;
class CComponent;

class CActor
{
protected:
    CWorld* world;
    Transform transform;
    std::vector<CComponent*> components;
    bool isPendingKill;
    bool isActive;


public:
    CActor(CWorld* world);
    virtual ~CActor();

    // Lifecycle methods
    virtual void BeginPlay();
    virtual void Tick(float deltaTime);
    virtual void Destroy();

    // Collision callback - override in derived classes
    virtual void OnCollision(CActor* other) {}

    // Component management
    template<typename T>
    T* AddComponent()
    {
        T* comp = new T(this);
        components.push_back(comp);
        comp->BeginPlay();
        return comp;
    }

    template<typename T>
    T* GetComponent()
    {
        for (auto comp : components) {
            T* castComp = dynamic_cast<T*>(comp);
            if (castComp) {
                return castComp;
            }
        }
        return nullptr;
    }

    // Transform accessors
    const Transform& GetTransform() const { return transform; }
    void SetTransform(const Transform& t) { transform = t; }

    Vector2 GetPosition() const { return transform.position; }
    void SetPosition(const Vector2& pos) { transform.position = pos; }

    float GetRotation() const { return transform.rotation; }
    void SetRotation(float rot) { transform.rotation = rot; }

    Vector2 GetScale() const { return transform.scale; }
    void SetScale(const Vector2& scale) { transform.scale = scale; }

    // State management
    bool IsPendingKill() const { return isPendingKill; }
    bool IsActive() const { return isActive; }
    void SetActive(bool active) { isActive = active; }

    // World access
    CWorld* GetWorld() const { return world; }
};