#pragma once
#include <cstdint>

// Engine-owned input enums so game code does not need SDL keycodes/buttons.
enum class Key : uint16_t
{
    Unknown = 0,
    Left,
    Right,
    Up,
    Down,
    A,
    D,
    W,
    S,
    Space,
    Escape
};

enum class GamepadButton : uint8_t
{
    South = 0, // A on Xbox / Cross on PlayStation
};

enum class GamepadAxis : uint8_t
{
    LeftX = 0,
    LeftY = 1,
};

