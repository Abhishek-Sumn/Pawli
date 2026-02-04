#pragma once
#include <cmath>

namespace Utils {
    struct Vec2 {
        float x, y;

        Vec2() : x(0), y(0) {}
        Vec2(float x, float y) : x(x), y(y) {}

        Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
        Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
        Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
        Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
        Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }

        float Length() const { return sqrtf(x * x + y * y); }
        float LengthSquared() const { return x * x + y * y; }

        Vec2 Normalized() const {
            float len = Length();
            if (len > 0.0001f) return Vec2(x / len, y / len);
            return Vec2(0, 0);
        }

        static float Distance(const Vec2& a, const Vec2& b) {
            return (a - b).Length();
        }

        static Vec2 Lerp(const Vec2& a, const Vec2& b, float t) {
            return Vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
        }
    };

    inline float Clamp(float value, float min, float max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    // Ease out quad
    inline float EaseOut(float t) {
        return t * (2 - t);
    }
}
