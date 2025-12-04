#include "pch.h"
#include "CPhysicsComponent.h"
#include "CActor.h"
#include "CWorld.h"
#include <SDL3/SDL.h>

CPhysicsComponent::CPhysicsComponent(CActor* owner, BodyType type)
    : CComponent(owner), bodyType(type),
    collisionCategory(0x0001), collisionMask(0xFFFF)
{
    bodyId = b2_nullBodyId;
    shapeId = b2_nullShapeId;
}

CPhysicsComponent::~CPhysicsComponent() {}

void CPhysicsComponent::BeginPlay()
{
    CWorld* world = owner->GetWorld();
    if (!world) return;

    b2WorldId worldId = world->GetPhysicsWorld();
    if (B2_IS_NULL(worldId)) return;

    // Create body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    Vector2 pos = owner->GetPosition();
    bodyDef.position = world->ToMeters(pos);

    // Set body type
    switch (bodyType)
    {
    case BodyType::Static:    bodyDef.type = b2_staticBody; break;
    case BodyType::Kinematic: bodyDef.type = b2_kinematicBody; break;
    case BodyType::Dynamic:   bodyDef.type = b2_dynamicBody; break;
    }

    bodyId = b2CreateBody(worldId, &bodyDef);

    // CRITICAL: Store actor pointer for collision callbacks
    b2Body_SetUserData(bodyId, owner);



    SDL_Log("✓ Physics body created for actor at (%.1f, %.1f)", pos.x, pos.y);
}

void CPhysicsComponent::Tick(float deltaTime)
{
    if (B2_IS_NULL(bodyId) || !isActive) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    // Sync actor position with physics
    b2Vec2 b2Pos = b2Body_GetPosition(bodyId);
    owner->SetPosition(world->ToPixels(b2Pos));

    b2Rot rotation = b2Body_GetRotation(bodyId);
    owner->SetRotation(b2Rot_GetAngle(rotation) * 180.0f / 3.14159f);


}

void CPhysicsComponent::Destroy()
{
    if (!B2_IS_NULL(shapeId))
    {
        b2DestroyShape(shapeId, false);
        shapeId = b2_nullShapeId;
    }

    if (!B2_IS_NULL(bodyId))
    {
        b2DestroyBody(bodyId);
        bodyId = b2_nullBodyId;
    }
}

void CPhysicsComponent::SetFixedRotation(bool fixed)
{
    if (B2_IS_NULL(bodyId)) return;

    // Use motion locks to fix rotation
    b2MotionLocks locks = b2Body_GetMotionLocks(bodyId);
    locks.angularZ = fixed;  // Lock angular rotation around Z-axis
    b2Body_SetMotionLocks(bodyId, locks);
}

void CPhysicsComponent::CreateBoxShape(float width, float height, bool isSensor)
{
    if (B2_IS_NULL(bodyId)) return;

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

    shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);

    SDL_Log("✓ Box shape created: %.1fx%.1f, Sensor=%d, Cat=0x%04X, Mask=0x%04X",
        width, height, isSensor, collisionCategory, collisionMask);
}

void CPhysicsComponent::CreateCircleShape(float radius, bool isSensor)
{
    if (B2_IS_NULL(bodyId)) return;

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

    shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);

    SDL_Log("✓ Circle shape created: radius=%.1f, Sensor=%d", radius, isSensor);
}

void CPhysicsComponent::SetCollisionFilter(uint32_t category, uint32_t mask)
{
    collisionCategory = category;
    collisionMask = mask;

    // Update existing shape if created
    if (!B2_IS_NULL(shapeId))
    {
        b2Filter filter;
        filter.categoryBits = category;
        filter.maskBits = mask;
        filter.groupIndex = 0;
        b2Shape_SetFilter(shapeId, filter);
    }
}

void CPhysicsComponent::SetVelocity(const Vector2& velocity)
{
    if (B2_IS_NULL(bodyId)) return;

    CWorld* world = owner->GetWorld();
    if (!world) return;

    b2Body_SetLinearVelocity(bodyId, world->ToMeters(velocity));
}

Vector2 CPhysicsComponent::GetVelocity() const
{
    if (B2_IS_NULL(bodyId)) return Vector2(0.0f, 0.0f);

    CWorld* world = owner->GetWorld();
    if (!world) return Vector2(0.0f, 0.0f);

    return world->ToPixels(b2Body_GetLinearVelocity(bodyId));
}