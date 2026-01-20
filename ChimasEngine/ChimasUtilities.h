#pragma once

#include <cmath>
#include <cstdint>

    struct Vector2 {
        float x, y;
        Vector2() : x(0.0f), y(0.0f) {}
        Vector2(float x, float y) : x(x), y(y) {}

        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }
        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }
        Vector2 operator*(float scalar) const {
            return Vector2(x * scalar, y * scalar);
        }
        float Length() const {
            return std::sqrt(x * x + y * y);
        }
        Vector2 Normalize() const {
            float len = Length();
            if (len > 0.0f) {
                return Vector2(x / len, y / len);
            }
            return Vector2(0.0f, 0.0f);
        }



    };
    struct Color {
        uint8_t r, g, b, a;
        Color() : r(255), g(255), b(255), a(255) {}
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
        static Color Magenta() { return Color(255, 0, 255, 255); }
        static Color White() { return Color(255, 255, 255, 255); }
        static Color Black() { return Color(0, 0, 0, 255); }

    };
    struct Transform {
        Vector2 position;
        float rotation;
        Vector2 scale;
        Transform() : position(0.0f, 0.0f), rotation(0.0f), scale(1.0f, 1.0f) {}
    };

    // Engine-owned rectangle type (avoid leaking SDL_FRect into game code)
    struct RectF {
        float x, y, w, h;
        RectF() : x(0), y(0), w(0), h(0) {}
        RectF(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    };

    // Engine-owned flip mode (avoid leaking SDL_FlipMode into game code)
    enum class FlipMode : uint8_t {
        None = 0,
        Horizontal = 1,
        Vertical = 2
    };
