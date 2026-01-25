#pragma once
#include <vector>
#include "ChimasUtilities.h"
#include "ChimasPhysicsHandles.h"

// Forward declarations
class CActor;
class CEngine;
class CEngineRender;
class CPhysicsComponent;
class CHUD;

class CWorld
{
private:
    CEngine* engine;
    CHUD* hud;
    std::vector<CActor*> actors;
    std::vector<CActor*> pendingActors;

    // Hide Box2D implementation details from game code (PIMPL)
    struct WorldImpl;
    WorldImpl* impl;

    float pixelsPerMeter;
    Vector2 worldBounds; // Screen size

    // Collision tracking
    struct CollisionPair {
        CActor* actorA;
        CActor* actorB;
    };
    std::vector<CollisionPair> collisionPairs;

public:
    CWorld(CEngine* engineRef,float x, float y);
    ~CWorld();

    bool GameRunning;

    // Actor management
    template<typename T>
    T* SpawnActor()
    {
        T* actor = new T(this);
        pendingActors.push_back(actor);
        return actor;
    }

    void DestroyActor(CActor* actor);

    // Lifecycle - Unity-like execution order
    void InitializeNewActors();
    void BeginPlay();
    void Tick(float deltaTime);
    void Render(CEngineRender* renderer);

    // Cleanup
    void CleanupDestroyedActors();

    void GameOver(bool end);

    // Engine access
    CEngine* GetEngine() const { return engine; }
    void SetEngine(CEngine* engineRef) { engine = engineRef; }

    CHUD* GetHUD() const { return hud; }

    // Physics
    void InitializePhysics(const Vector2& gravity = Vector2(0.0f, 0.0f));
    void StepPhysics(float deltaTime);
    void CreateBoundaryWalls(float width, float height);
    void ProcessCollisions(); // NEW: Process collision events

    float GetPixelsPerMeter() const { return pixelsPerMeter; }
    Vector2 GetWorldBounds() const { return worldBounds; }

    // Conversion helpers
    Vector2 ToPixels(Vector2 meters) const;
    Vector2 ToMeters(Vector2 pixels) const;
    float ToPixels(float meters) const;
    float ToMeters(float pixels) const;

    // Utility
    const std::vector<CActor*>& GetActors() const { return actors; }

private:
    // Engine-internal hook (world id is owned by engine, but exposed to engine code only)
    PhysicsWorldHandle GetPhysicsWorldHandle() const;
    friend class CPhysicsComponent;
};