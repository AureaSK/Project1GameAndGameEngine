#pragma once
#include "CComponent.h"
#include "CSpriteComponent.h"
#include <vector>
#include <string>

struct AnimationFrame
{
    int row;
    int col;
    float duration;
};

struct Animation
{
    std::string name;
    std::vector<AnimationFrame> frames;
    bool loop;
};

class CAnimationComponent : public CComponent
{
private:
    CSpriteComponent* sprite;
    std::vector<Animation> animations;
    int currentAnimation;
    int currentFrame;
    float frameTimer;
    int tileWidth;
    int tileHeight;
    bool isPlaying;

public:

    CAnimationComponent(CActor* owner);
    virtual ~CAnimationComponent();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    void AddAnimation(const std::string& name, const std::vector<AnimationFrame>& frames, bool loop = true);
    void PlayAnimation(const std::string& name);
    void StopAnimation();
    bool IsAnimationPlaying(const std::string& name) const;

    void SetTileSize(int tileW, int tileH) {
        tileWidth = tileW;
        tileHeight = tileH;
    }

    int GetTileWidth() const { return tileWidth; }
    int GetTileHeight() const { return tileHeight; }
    std::string GetCurrentAnimationName() const;
};