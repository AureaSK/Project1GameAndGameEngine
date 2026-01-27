#include "pch.h"
#include "ParallaxLayer.h"
#include "CSpriteComponent.h"
#include "CWorld.h"
#include "ChimasLog.h"

ParallaxLayer::ParallaxLayer(CWorld* world)
    : CActor(world), sprite(nullptr), scrollSpeed(1.0f), currentOffset(0.0f), layerHeight(800.0f)
{
}

ParallaxLayer::~ParallaxLayer()
{
}

void ParallaxLayer::BeginPlay()
{
    CActor::BeginPlay();
}

void ParallaxLayer::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    // Scroll downward
    currentOffset += scrollSpeed * deltaTime;

    // Wrap around when we've scrolled a full layer height
    if (currentOffset >= layerHeight)
    {
        currentOffset -= layerHeight;
    }

    // Update actor position for vertical scrolling
    Vector2 pos = GetPosition();
    pos.y = currentOffset;
    SetPosition(pos);
}

bool ParallaxLayer::Setup(const std::string& texturePath, int srcX, int srcY, int srcWidth, int srcHeight, float speed)
{
    scrollSpeed = speed;
    layerHeight = (float)srcHeight;

    // Create sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (!sprite->LoadTexture(texturePath))
    {
        //ChimasLog::Error("ParallaxLayer: Failed to load texture: %s", texturePath.c_str());
        return false;
    }

    // Set source rectangle from sprite sheet
    sprite->SetSourceRect((float)srcX, (float)srcY, (float)srcWidth, (float)srcHeight);

    // Set size to cover the world
    Vector2 worldBounds = world->GetWorldBounds();
    sprite->SetSize(worldBounds.x, (float)srcHeight);

    ChimasLog::Info("ParallaxLayer setup complete: speed=%.2f", speed);
    return true;
}