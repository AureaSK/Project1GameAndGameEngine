#include "pch.h"
#include "CAnimationComponent.h"
#include "CActor.h"

// Change constructor to match header
CAnimationComponent::CAnimationComponent(CActor* owner)
    : CComponent(owner), sprite(nullptr), currentAnimation(-1),
    currentFrame(0), frameTimer(0.0f), tileWidth(0),
    tileHeight(0), isPlaying(false)
{
}

CAnimationComponent::~CAnimationComponent()
{
}

void CAnimationComponent::BeginPlay()
{
    // Find sprite component on same actor
    sprite = owner->GetComponent<CSpriteComponent>();

    if (!sprite)
    {
        SDL_Log("CAnimationComponent: Warning - No sprite component found!");
    }
    else if (tileWidth == 0 || tileHeight == 0)
    {
        SDL_Log("CAnimationComponent: Warning - Tile size not set!");
    }
}

// ... rest of the existing CAnimationComponent.cpp code remains the same ...

void CAnimationComponent::Tick(float deltaTime)
{
    if (!isPlaying || currentAnimation < 0 || !sprite)
        return;

    Animation& anim = animations[currentAnimation];

    if (anim.frames.empty())
        return;

    frameTimer += deltaTime;

    AnimationFrame& frame = anim.frames[currentFrame];

    if (frameTimer >= frame.duration)
    {
        frameTimer = 0.0f;
        currentFrame++;

        if (currentFrame >= anim.frames.size())
        {
            if (anim.loop)
            {
                currentFrame = 0;
            }
            else
            {
                currentFrame = anim.frames.size() - 1;
                isPlaying = false;
                return;
            }
        }

        // Update sprite source rect
        frame = anim.frames[currentFrame];
        float x = frame.col * tileWidth;
        float y = frame.row * tileHeight;
        sprite->SetSourceRect(x, y, static_cast<float>(tileWidth), static_cast<float>(tileHeight));
    }
}

void CAnimationComponent::AddAnimation(const std::string& name, const std::vector<AnimationFrame>& frames, bool loop)
{
    Animation anim;
    anim.name = name;
    anim.frames = frames;
    anim.loop = loop;
    animations.push_back(anim);

    SDL_Log("Added animation '%s' with %zu frames", name.c_str(), frames.size());
}

void CAnimationComponent::PlayAnimation(const std::string& name)
{
    for (size_t i = 0; i < animations.size(); i++)
    {
        if (animations[i].name == name)
        {
            if (currentAnimation != i)
            {
                currentAnimation = static_cast<int>(i);
                currentFrame = 0;
                frameTimer = 0.0f;
                isPlaying = true;

                // Set initial frame
                if (sprite && !animations[i].frames.empty())
                {
                    AnimationFrame& frame = animations[i].frames[0];
                    float x = frame.col * tileWidth;
                    float y = frame.row * tileHeight;
                    sprite->SetSourceRect(x, y, static_cast<float>(tileWidth), static_cast<float>(tileHeight));
                    sprite->SetSize(static_cast<float>(tileWidth), static_cast<float>(tileHeight));
                }
            }
            return;
        }
    }

    SDL_Log("CAnimationComponent: Animation '%s' not found!", name.c_str());
}

void CAnimationComponent::StopAnimation()
{
    isPlaying = false;
}

bool CAnimationComponent::IsAnimationPlaying(const std::string& name) const
{
    if (currentAnimation < 0 || !isPlaying)
        return false;

    return animations[currentAnimation].name == name;
}

std::string CAnimationComponent::GetCurrentAnimationName() const
{
    if (currentAnimation >= 0 && currentAnimation < animations.size())
    {
        return animations[currentAnimation].name;
    }
    return "";
}