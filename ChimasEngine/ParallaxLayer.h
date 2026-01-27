#pragma once
#include <string>

#include "CActor.h"

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

    // Setup the layer
    bool Setup(const std::string& texturePath, int srcX, int srcY, int srcWidth, int srcHeight, float speed);

    // Set the scroll speed multiplier
    void SetScrollSpeed(float speed) { scrollSpeed = speed; }
};