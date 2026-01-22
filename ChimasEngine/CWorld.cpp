#include "pch.h"
#include "CWorld.h"
#include "CActor.h"
#include "CEngineRender.h"
#include "CSpriteComponent.h"
#include "CPhysicsComponent.h"
#include <box2d.h>
#include <iostream>

struct CWorld::WorldImpl
{
    b2WorldId worldId = b2_nullWorldId;
};

CWorld::CWorld(CEngine* engineRef, float x, float y)
    : engine(engineRef), impl(new WorldImpl()), pixelsPerMeter(50.0f), worldBounds(x, y), GameRunning(true)
{
    SDL_Log("CWorld created");
}

CWorld::~CWorld()
{
    for (auto actor : actors) {
        delete actor;
    }
    actors.clear();

    for (auto actor : pendingActors) {
        delete actor;
    }
    pendingActors.clear();

    // Destroy Box2D world
    if (impl && !B2_IS_NULL(impl->worldId))
    {
        b2DestroyWorld(impl->worldId);
        impl->worldId = b2_nullWorldId;
    }

    delete impl;
    impl = nullptr;
}

void CWorld::InitializePhysics(const Vector2& gravity)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { gravity.x, gravity.y };

    // NOTE: In Box2D v3.1, contact events are enabled PER SHAPE, not on the world
    // See b2ShapeDef.enableContactEvents

    impl->worldId = b2CreateWorld(&worldDef);

    if (B2_IS_NULL(impl->worldId)) {
        SDL_Log("FAILED to create Box2D world!");
    }
    else {
        SDL_Log("Box2D World Created Successfully! (PPM: %.1f)", pixelsPerMeter);
    }
}

void CWorld::StepPhysics(float deltaTime)
{
    if (!impl || B2_IS_NULL(impl->worldId)) return;

    int32_t subStepCount = 4;
    b2World_Step(impl->worldId, deltaTime, subStepCount);
}

void CWorld::ProcessCollisions()
{
    if (!impl || B2_IS_NULL(impl->worldId)) return;

    collisionPairs.clear();

    // Get all contact events
    b2ContactEvents events = b2World_GetContactEvents(impl->worldId);

    // Process begin touch events
    for (int i = 0; i < events.beginCount; i++)
    {
        b2ContactBeginTouchEvent* evt = &events.beginEvents[i];

        // Get bodies from shapes
        b2BodyId bodyA = b2Shape_GetBody(evt->shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(evt->shapeIdB);

        // Get actor pointers
        CActor* actorA = static_cast<CActor*>(b2Body_GetUserData(bodyA));
        CActor* actorB = static_cast<CActor*>(b2Body_GetUserData(bodyB));

        // Validate and add pair
        if (actorA && actorB && !actorA->IsPendingKill() && !actorB->IsPendingKill())
        {
            collisionPairs.push_back({ actorA, actorB });
        }
    }

    // Process hit events (high-speed collisions)
    for (int i = 0; i < events.hitCount; i++)
    {
        b2ContactHitEvent* evt = &events.hitEvents[i];

        b2BodyId bodyA = b2Shape_GetBody(evt->shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(evt->shapeIdB);

        CActor* actorA = static_cast<CActor*>(b2Body_GetUserData(bodyA));
        CActor* actorB = static_cast<CActor*>(b2Body_GetUserData(bodyB));

        if (actorA && actorB && !actorA->IsPendingKill() && !actorB->IsPendingKill())
        {
            // Check if not already added
            bool exists = false;
            for (const auto& pair : collisionPairs)
            {
                if ((pair.actorA == actorA && pair.actorB == actorB) ||
                    (pair.actorA == actorB && pair.actorB == actorA))
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
            {
                collisionPairs.push_back({ actorA, actorB });
            }
        }
    }

    for (const auto& pair : collisionPairs)
    {
        // Check if actorA is still valid before calling OnCollision
        if (!pair.actorA->IsPendingKill())
        {
            pair.actorA->OnCollision(pair.actorB);
        }

        // CRITICAL: Check BOTH actors before second callback
        if (!pair.actorA->IsPendingKill() && !pair.actorB->IsPendingKill())
        {
            pair.actorB->OnCollision(pair.actorA);
        }
    }
}

void CWorld::CreateBoundaryWalls(float width, float height)
{
   

    if (!impl || B2_IS_NULL(impl->worldId))
    {
        SDL_Log("Error: Physics world not initialized!");
        return;
    }

    worldBounds = Vector2(width, height);

    float wallThickness = 1.0f; // meters
    float halfWidth = width / (2.0f * pixelsPerMeter);
    float halfHeight = height / (2.0f * pixelsPerMeter);

    // Create static body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;

    // CRITICAL: Create SOLID walls with contact events ENABLED
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.enableContactEvents = true;  // THIS LINE!
    shapeDef.isSensor = false;
    shapeDef.density = 0.0f;


    // Set collision filter for walls
    shapeDef.filter.categoryBits = CollisionCategory::WALL;
    shapeDef.filter.maskBits = CollisionCategory::PLAYER | CollisionCategory::ENEMY;

    // Bottom wall
    bodyDef.position = { halfWidth, height / pixelsPerMeter + wallThickness / 2.0f };
    b2BodyId bottomBody = b2CreateBody(impl->worldId, &bodyDef);
    b2Polygon bottomBox = b2MakeBox(halfWidth, wallThickness / 2.0f);
    b2CreatePolygonShape(bottomBody, &shapeDef, &bottomBox);
    SDL_Log("✓ Bottom wall created");

    // Top wall
    bodyDef.position = { halfWidth, -wallThickness / 2.0f };
    b2BodyId topBody = b2CreateBody(impl->worldId, &bodyDef);
    b2Polygon topBox = b2MakeBox(halfWidth, wallThickness / 2.0f);
    b2CreatePolygonShape(topBody, &shapeDef, &topBox);
    SDL_Log("✓ Top wall created");

    // Left wall
    bodyDef.position = { -wallThickness / 2.0f, halfHeight };
    b2BodyId leftBody = b2CreateBody(impl->worldId, &bodyDef);
    b2Polygon leftBox = b2MakeBox(wallThickness / 2.0f, halfHeight);
    b2CreatePolygonShape(leftBody, &shapeDef, &leftBox);
    SDL_Log("✓ Left wall created");

    // Right wall
    bodyDef.position = { width / pixelsPerMeter + wallThickness / 2.0f, halfHeight };
    b2BodyId rightBody = b2CreateBody(impl->worldId, &bodyDef);
    b2Polygon rightBox = b2MakeBox(wallThickness / 2.0f, halfHeight);
    b2CreatePolygonShape(rightBody, &shapeDef, &rightBox);
    SDL_Log("Right wall created");

    SDL_Log("All boundary walls created - Contact events ENABLED");
}

Vector2 CWorld::ToPixels(Vector2 meters) const
{
    return Vector2(meters.x * pixelsPerMeter, meters.y * pixelsPerMeter);
}

Vector2 CWorld::ToMeters(Vector2 pixels) const
{
    return Vector2(pixels.x / pixelsPerMeter, pixels.y / pixelsPerMeter);
}

float CWorld::ToPixels(float meters) const
{
    return meters * pixelsPerMeter;
}

float CWorld::ToMeters(float pixels) const
{
    return pixels / pixelsPerMeter;
}

PhysicsWorldHandle CWorld::GetPhysicsWorldHandle() const
{
    PhysicsWorldHandle h;
    if (!impl) return h;
    h.index1 = impl->worldId.index1;
    h.generation = impl->worldId.generation;
    return h;
}

void CWorld::DestroyActor(CActor* actor)
{
    if (actor) {
        actor->Destroy();
    }
}

void CWorld::InitializeNewActors()
{
    if (!pendingActors.empty())
    {
        SDL_Log("Initializing %zu new actors...", pendingActors.size());

        for (auto actor : pendingActors)
        {
            actors.push_back(actor);
            actor->BeginPlay();

            SDL_Log(" Actor spawned and initialized at position (%.1f, %.1f)",
                actor->GetPosition().x, actor->GetPosition().y);
        }
        pendingActors.clear();
    }
}

void CWorld::BeginPlay()
{
    GameRunning = true;
    SDL_Log("=== World BeginPlay ===");
    InitializeNewActors();
    SDL_Log("=== World Ready ===");
}

void CWorld::Tick(float deltaTime)
{
    // EXECUTION ORDER (Unity-like):
    // 1. Process new actors (Awake/OnEnable)
    InitializeNewActors();

    // 2. Update actors (Update)
    for (auto actor : actors)
    {
        if (actor->IsActive() && !actor->IsPendingKill())
        {
            actor->Tick(deltaTime);
        }
    }

    // 3. Physics step (FixedUpdate)
    StepPhysics(deltaTime);

    // 4. Collision callbacks (OnCollision)
    ProcessCollisions();

    // 5. Cleanup destroyed actors (OnDestroy)
    CleanupDestroyedActors();

    GameOver(GameRunning);
}

void CWorld::Render(CEngineRender* renderer)
{
    if (!renderer) return;

    // Render all actors with sprite components
    for (auto actor : actors)
    {
        if (actor->IsActive() && !actor->IsPendingKill())
        {
            CSpriteComponent* sprite = actor->GetComponent<CSpriteComponent>();
            if (sprite && sprite->IsActive())
            {
                sprite->Render(actor->GetPosition(), actor->GetRotation());
            }
        }
    }
}

void CWorld::CleanupDestroyedActors()
{
    auto it = actors.begin();
    while (it != actors.end())
    {
        if ((*it)->IsPendingKill())
        {
            SDL_Log("Destroying actor at position (%.1f, %.1f)",
                (*it)->GetPosition().x, (*it)->GetPosition().y);
            delete* it;
            it = actors.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CWorld::GameOver(bool end)
{
    if (!end)
    {
        engine->EndGame();
    }
}
