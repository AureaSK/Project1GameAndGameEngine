#pragma once

#include "CActor.h"
#include <string>

class CSpriteComponent;

// Simple parallax layer - one actor per layer
class ParallaxLayer : public CActor
{
private:
    CSpriteComponent* sprite;
    float scrollSpeed;
    float currentOffset;
    float layerHeight;

public:
    ParallaxLayer(CWorld* world);
    virtual ~ParallaxLayer();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    // Setup the layer with optional scale
    bool Setup(const std::string& texturePath, int srcX, int srcY, int srcWidth, int srcHeight, float speed, float scale = 1.0f);

    // Set the scroll speed multiplier
    void SetScrollSpeed(float speed) { scrollSpeed = speed; }
};