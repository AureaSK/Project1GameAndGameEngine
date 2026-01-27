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
    // Layers are cleaned up by CWorld
}

void ParallaxBackground::BeginPlay()
{
    CActor::BeginPlay();
    ChimasLog::Info("ParallaxBackground initialized with %zu layers", layers.size());
}

void ParallaxBackground::Tick(float deltaTime)
{
    CActor::Tick(deltaTime);

    // Update all layer speeds if base speed changes
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
    float speedMultiplier, float posX, float posY)
{
    if (spriteSheetPath.empty())
    {
        //ChimasLog::Error("Must call LoadSpriteSheet before adding layers!");
        return false;
    }

    // Spawn a new ParallaxLayer actor
    ParallaxLayer* layer = world->SpawnActor<ParallaxLayer>();
    if (!layer)
    {
        //ChimasLog::Error("Failed to spawn ParallaxLayer actor!");
        return false;
    }

    // Calculate actual scroll speed for this layer
    float layerSpeed = baseScrollSpeed * speedMultiplier;

    // Setup the layer with sprite sheet section
    if (!layer->Setup(spriteSheetPath, srcX, srcY, srcWidth, srcHeight, layerSpeed))
    {
        //ChimasLog::Error("Failed to setup parallax layer!");
        layer->Destroy();
        return false;
    }

    // Position the layer at the specified position
    // Since we're using top-left pivot, this is now the EXACT position
    layer->SetPosition(Vector2(posX, posY));

    // Store reference
    layers.push_back(layer);

    ChimasLog::Info("Added parallax layer from section (%d, %d, %d, %d) at pos (%.1f, %.1f) - speed: %.2f",
        srcX, srcY, srcWidth, srcHeight, posX, posY, layerSpeed);

    return true;
}