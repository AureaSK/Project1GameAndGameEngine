#pragma once
#include "CComponent.h"
#include "ChimasUtilities.h"
#include "ChimasPhysicsHandles.h"

enum class BodyType
{
    Static,
    Kinematic,
    Dynamic
};

// Collision categories
namespace CollisionCategory
{
    const uint32_t PLAYER = 0x0001;
    const uint32_t ENEMY = 0x0002;
    const uint32_t PLAYER_PROJECTILE = 0x0004;
    const uint32_t ENEMY_PROJECTILE = 0x0008;
    const uint32_t WALL = 0x0010;
}

class CPhysicsComponent : public CComponent
{
private:
    // Hide Box2D body/shape ids from game code
    struct Impl;
    Impl* impl;

    BodyType bodyType;
    uint32_t collisionCategory;
    uint32_t collisionMask;

public:
    CPhysicsComponent(CActor* owner, BodyType type = BodyType::Dynamic);
    virtual ~CPhysicsComponent();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void Destroy() override;

    void SetFixedRotation(bool fixed);

    // Shape creation - ALWAYS enables contact events
    void CreateBoxShape(float width, float height, bool isSensor = false);
    void CreateCircleShape(float radius, bool isSensor = false);

    // Collision setup
    void SetCollisionFilter(uint32_t category, uint32_t mask);

    // Physics control
    void SetVelocity(const Vector2& velocity);
    Vector2 GetVelocity() const;
};