#pragma once
#include <vector>
#include <box2d.h>
#include "ChimasUtilities.h"

// Forward declarations
class CActor;
class CEngine;
class CEngineRender;

class CWorld
{
private:
    CEngine* engine;
    std::vector<CActor*> actors;
    std::vector<CActor*> pendingActors;

    

    // Box2D Physics
    b2WorldId worldId;
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

    // Physics
    void InitializePhysics(const Vector2& gravity = Vector2(0.0f, 0.0f));
    void StepPhysics(float deltaTime);
    void CreateBoundaryWalls(float width, float height);
    void ProcessCollisions(); // NEW: Process collision events

    b2WorldId GetPhysicsWorld() const { return worldId; }
    float GetPixelsPerMeter() const { return pixelsPerMeter; }
    Vector2 GetWorldBounds() const { return worldBounds; }

    // Conversion helpers
    Vector2 ToPixels(b2Vec2 meters) const;
    b2Vec2 ToMeters(Vector2 pixels) const;
    float ToPixels(float meters) const;
    float ToMeters(float pixels) const;

    // Utility
    const std::vector<CActor*>& GetActors() const { return actors; }
};