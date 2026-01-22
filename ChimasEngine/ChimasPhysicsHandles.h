#pragma once
#include <cstdint>

// Engine-owned opaque handles for physics objects (so game code doesn't see Box2D ids).
struct PhysicsWorldHandle
{
    uint16_t index1 = 0;
    uint16_t generation = 0;
};

