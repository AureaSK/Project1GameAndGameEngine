#include "pch.h"
#include "CPhysicsComponent.h"
#include "CActor.h"
#include "CWorld.h"
#include "ChimasLog.h"
#include <box2d.h>

struct CPhysicsComponent::Impl
{
    b2BodyId bodyId = b2_nullBodyId;
    b2ShapeId shapeId = b2_nullShapeId;
};

CPhysicsComponent::CPhysicsComponent(CActor* owner, BodyType type)
    : CComponent(owner), bodyType(type),
    collisionCategory(0x0001), collisionMask(0xFFFF)
{
    impl = new Impl();
}

CPhysicsComponent::~CPhysicsComponent()
{
    Destroy();
    delete impl;
    impl = nullptr;
}

void CPhysicsComponent::BeginPlay()
{
    CWorld* world = owner->GetWorld();
    if (!world) return;

    PhysicsWorldHandle h = world->GetPhysicsWorldHandle();
    b2WorldId worldId{ h.index1, h.generation };
    if (B2_IS_NULL(worldId)) return;

    // Create body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    Vector2 pos = owner->GetPosition();
    Vector2 posMeters = world->ToMeters(pos);
    bodyDef.position = { posMeters.x, posMeters.y };

    // Set body type
    switch (bodyType)
    {
    case BodyType::Static:    bodyDef.type = b2_staticBody; break;
    case BodyType::Kinematic: bodyDef.type = b2_kinematicBody; break;
    case BodyType::Dynamic:   bodyDef.type = b2_dynamicBody; break;
    }

    impl->bodyId = b2CreateBody(worldId, &bodyDef);

    // CRITICAL: Store actor pointer for collision callbacks
    b2Body_SetUserData(impl->bodyId, owner);



    ChimasLog::Info("Physics body created for actor at (%.1f, %.1f)", pos.x, pos.y);
}

void CPhysicsComponent::Tick(float deltaTime)
{
    if (!impl || B2_IS_NULL(impl->bodyId) || !isActive) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    // Sync actor position with physics
    b2Vec2 b2Pos = b2Body_GetPosition(impl->bodyId);
    owner->SetPosition(world->ToPixels(Vector2(b2Pos.x, b2Pos.y)));

    b2Rot rotation = b2Body_GetRotation(impl->bodyId);
    owner->SetRotation(b2Rot_GetAngle(rotation) * 180.0f / 3.14159f);


}

void CPhysicsComponent::Destroy()
{
    if (!impl) return;

    if (!B2_IS_NULL(impl->shapeId))
    {
        b2DestroyShape(impl->shapeId, false);
        impl->shapeId = b2_nullShapeId;
    }

    if (!B2_IS_NULL(impl->bodyId))
    {
        b2DestroyBody(impl->bodyId);
        impl->bodyId = b2_nullBodyId;
    }
}

void CPhysicsComponent::SetFixedRotation(bool fixed)
{
    if (!impl || B2_IS_NULL(impl->bodyId)) return;

    // Use motion locks to fix rotation
    b2MotionLocks locks = b2Body_GetMotionLocks(impl->bodyId);
    locks.angularZ = fixed;  // Lock angular rotation around Z-axis
    b2Body_SetMotionLocks(impl->bodyId, locks);
}

void CPhysicsComponent::CreateBoxShape(float width, float height, bool isSensor)
{
    if (!impl || B2_IS_NULL(impl->bodyId)) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    // Create polygon shape
    float halfW = world->ToMeters(width / 2.0f);
    float halfH = world->ToMeters(height / 2.0f);
    b2Polygon box = b2MakeBox(halfW, halfH);

    // Shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = isSensor ? 0.0f : 1.0f;
    shapeDef.isSensor = isSensor;

    // CRITICAL: Always enable contact events!
    shapeDef.enableContactEvents = true;

    // Apply collision filter
    shapeDef.filter.categoryBits = collisionCategory;
    shapeDef.filter.maskBits = collisionMask;

    impl->shapeId = b2CreatePolygonShape(impl->bodyId, &shapeDef, &box);

    ChimasLog::Info("Box shape created: %.1fx%.1f, Sensor=%d, Cat=0x%04X, Mask=0x%04X",
        width, height, isSensor, collisionCategory, collisionMask);
}

void CPhysicsComponent::CreateCircleShape(float radius, bool isSensor)
{
    if (!impl || B2_IS_NULL(impl->bodyId)) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    // Create circle shape
    b2Circle circle;
    circle.center = { 0.0f, 0.0f };
    circle.radius = world->ToMeters(radius);

    // Shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = isSensor ? 0.0f : 1.0f;
    shapeDef.isSensor = isSensor;

    // CRITICAL: Always enable contact events!
    shapeDef.enableContactEvents = true;

    // Apply collision filter
    shapeDef.filter.categoryBits = collisionCategory;
    shapeDef.filter.maskBits = collisionMask;

    impl->shapeId = b2CreateCircleShape(impl->bodyId, &shapeDef, &circle);

    ChimasLog::Info("Circle shape created: radius=%.1f, Sensor=%d", radius, isSensor);
}

void CPhysicsComponent::SetCollisionFilter(uint32_t category, uint32_t mask)
{
    collisionCategory = category;
    collisionMask = mask;

    // Update existing shape if created
    if (impl && !B2_IS_NULL(impl->shapeId))
    {
        b2Filter filter;
        filter.categoryBits = category;
        filter.maskBits = mask;
        filter.groupIndex = 0;
        b2Shape_SetFilter(impl->shapeId, filter);
    }
}

void CPhysicsComponent::SetVelocity(const Vector2& velocity)
{
    if (!impl || B2_IS_NULL(impl->bodyId)) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    Vector2 m = world->ToMeters(velocity);
    b2Body_SetLinearVelocity(impl->bodyId, b2Vec2{ m.x, m.y });
}

Vector2 CPhysicsComponent::GetVelocity() const
{
    if (!impl || B2_IS_NULL(impl->bodyId)) return Vector2(0.0f, 0.0f);

    CWorld* world = owner->GetWorld();
    if (!world) return Vector2(0.0f, 0.0f);

    b2Vec2 v = b2Body_GetLinearVelocity(impl->bodyId);
    return world->ToPixels(Vector2(v.x, v.y));
}

void CPhysicsComponent::SetPosition(const Vector2& position)
{
    if (!impl || B2_IS_NULL(impl->bodyId)) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    Vector2 m = world->ToMeters(position);

    // Get current rotation to preserve it
    b2Rot currentRotation = b2Body_GetRotation(impl->bodyId);

    // Set transform with both position and rotation
    b2Body_SetTransform(impl->bodyId, b2Vec2{ m.x, m.y }, currentRotation);
}