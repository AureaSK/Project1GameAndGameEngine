#include "pch.h"
#include "ParallaxBackground.h"
#include "ParallaxLayer.h"
#include "CWorld.h"
#include "ChimasLog.h"

ParallaxBackground::ParallaxBackground(CWorld* world)
    : CActor(world), baseScrollSpeed(50.0f)
{
}

ParallaxBackground::~ParallaxBackground()
{
    
}

void ParallaxBackground::BeginPlay()
{
    CActor::BeginPlay();
    ChimasLog::Info("ParallaxBackground initialized with %zu layers", layers.size());
}

void ParallaxBackground::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    for (ParallaxLayer* layer : layers)
    {
        if (layer)
        {
            // Each layer already handles its own scrolling in its Tick method
        }
    }
}

bool ParallaxBackground::LoadSpriteSheet(const std::string& texturePath)
{
    spriteSheetPath = texturePath;
    ChimasLog::Info("Parallax sprite sheet path set to: %s", spriteSheetPath.c_str());
    return true;
}

bool ParallaxBackground::AddLayerFromSection(int srcX, int srcY, int srcWidth, int srcHeight,
    float speedMultiplier, float posX, float posY, float scale)
{
    if (spriteSheetPath.empty())
    {
        return false;
    }

    // Spawn a new ParallaxLayer actor
    ParallaxLayer* layer = world->SpawnActor<ParallaxLayer>();
    if (!layer)
    {
        return false;
    }

    float layerSpeed = baseScrollSpeed * speedMultiplier;

    if (!layer->Setup(spriteSheetPath, srcX, srcY, srcWidth, srcHeight, layerSpeed, scale))
    {
        layer->Destroy();
        return false;
    }

    layer->SetPosition(Vector2(posX, posY));

    layers.push_back(layer);

    ChimasLog::Info("Added parallax layer from section (%d, %d, %d, %d) at pos (%.1f, %.1f) - speed: %.2f, scale: %.2f",
        srcX, srcY, srcWidth, srcHeight, posX, posY, layerSpeed, scale);

    return true;
}