#pragma once

namespace Platform {

struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }
    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2& other) const { return !(*this == other); }
    
    static Vector2 Zero() { return Vector2(0, 0); }
};

struct Point {
    int X, Y;
    Point() : X(0), Y(0) {}
    Point(int x, int y) : X(x), Y(y) {}
};

struct Rectangle {
    int X, Y, Width, Height;
    Rectangle() : X(0), Y(0), Width(0), Height(0) {}
    Rectangle(int x, int y, int w, int h) : X(x), Y(y), Width(w), Height(h) {}
};

struct Color {
    uint8_t R, G, B, A;
    Color() : R(255), G(255), B(255), A(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) : R(r), G(g), B(b), A(a) {}
};

} // namespace Platform

// Convenience aliases for InputManager
using Vector2 = Platform::Vector2;
