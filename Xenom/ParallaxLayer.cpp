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

    currentOffset += scrollSpeed * deltaTime;

    Vector2 worldBounds = world->GetWorldBounds();
    float screenHeight = worldBounds.y;

    if (currentOffset >= screenHeight)
    {
        currentOffset = -layerHeight;
    }

    Vector2 pos = GetPosition();
    pos.y = currentOffset;
    SetPosition(pos);
}

bool ParallaxLayer::Setup(const std::string& texturePath, int srcX, int srcY, int srcWidth, int srcHeight, float speed, float scale)
{
    scrollSpeed = speed;

    // Create sprite component
    sprite = AddComponent<CSpriteComponent>();
    if (!sprite->LoadTexture(texturePath))
    {
        ChimasLog::Info("ParallaxLayer: Failed to load texture: %s", texturePath.c_str());
        return false;
    }

    // Set source rectangle from sprite sheet
    sprite->SetSourceRect((float)srcX, (float)srcY, (float)srcWidth, (float)srcHeight);

    // Apply scale to dimensions
    float scaledWidth = (float)srcWidth * scale;
    float scaledHeight = (float)srcHeight * scale;

    sprite->SetSize(scaledWidth, scaledHeight);

 
    layerHeight = scaledHeight;

    sprite->UseTopLeftPositioning(true);

    currentOffset = -layerHeight;

    ChimasLog::Info("ParallaxLayer setup: original=(%dx%d), scaled=(%.0fx%.0f), scale=%.2f, speed=%.2f, startPos=%.0f",
        srcWidth, srcHeight, scaledWidth, scaledHeight, scale, speed, currentOffset);
    return true;
}