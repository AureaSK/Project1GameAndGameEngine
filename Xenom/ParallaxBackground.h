#pragma once

#include "CActor.h"
#include <vector>
#include <string>

class ParallaxLayer;

class ParallaxBackground : public CActor
{
private:
    std::vector<ParallaxLayer*> layers;
    float baseScrollSpeed;
    std::string spriteSheetPath;

public:
    ParallaxBackground(CWorld* world);
    virtual ~ParallaxBackground();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    // Load the sprite sheet that contains all parallax layers
    bool LoadSpriteSheet(const std::string& texturePath);

    // Add a parallax layer from a section of the sprite sheet
    // srcX, srcY: top-left corner of the section in the sprite sheet
    // srcWidth, srcHeight: size of the section to use
    // speedMultiplier: how fast this layer scrolls relative to base speed
    // posX, posY: position in world space (defaults to 0, 0)
    bool AddLayerFromSection(int srcX, int srcY, int srcWidth, int srcHeight,
        float speedMultiplier, float posX = 0.0f, float posY = 0.0f);

    // Set the base scroll speed (all layers scale from this)
    void SetBaseScrollSpeed(float speed) { baseScrollSpeed = speed; }

    // Get current scroll speed
    float GetBaseScrollSpeed() const { return baseScrollSpeed; }
};