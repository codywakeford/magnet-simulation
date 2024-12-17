#include <stdexcept> // For exceptions
#include <cmath>     // For sqrt()

struct Vec2 {
    float x, y;

    Vec2(float x = 0, float y = 0) : x(x), y(y) {}

    Vec2 operator+ (const Vec2& other) {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator- (const Vec2& other) {
        return Vec2(x - other.x, y - other.y);
    }


    Vec2 operator* (const Vec2& other) {
        return Vec2(x * other.x, y * other.y);
    }

    Vec2 operator* (float other) {
        return Vec2(x * other, y * other);
    }

    Vec2 operator/ (const Vec2& other) {
        if (other.x == 0 || other.y == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return Vec2(x / other.x, y / other.y);
    }

    Vec2 operator/ (const Vec2& other) const {
        if (other.x == 0 || other.y == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return Vec2(x / other.x, y / other.y);
    }

    Vec2 operator/ (float other) const {
        if (other == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return Vec2(x / other, y / other);
    }

    // Magnitude (Length)
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalization
    Vec2 normalize() const {
        float mag = magnitude();
        if (mag == 0) {
            throw std::invalid_argument("Cannot normalize a zero vector");
        }
        return *this / mag;
    }

};