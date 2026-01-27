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

    bool LoadSpriteSheet(const std::string& texturePath);

    bool AddLayerFromSection(int srcX, int srcY, int srcWidth, int srcHeight,
        float speedMultiplier, float posX = 0.0f, float posY = 0.0f, float scale = 1.0f);

    void SetBaseScrollSpeed(float speed) { baseScrollSpeed = speed; }

    float GetBaseScrollSpeed() const { return baseScrollSpeed; }
};