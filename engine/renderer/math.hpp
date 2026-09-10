#pragma once

#include <cmath>

namespace gta4web::math {

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    constexpr Vec3 operator+(Vec3 other) const { return {x + other.x, y + other.y, z + other.z}; }
    constexpr Vec3 operator-(Vec3 other) const { return {x - other.x, y - other.y, z - other.z}; }
    constexpr Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    constexpr Vec3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    Vec3& operator+=(Vec3 other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(Vec3 other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
};

constexpr Vec3 operator*(float scalar, Vec3 value) { return value * scalar; }

inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float length_squared(Vec3 value) { return dot(value, value); }
inline float length(Vec3 value) { return std::sqrt(length_squared(value)); }

struct Mat4 {
    float m[16]{};

    static Mat4 identity();
    static Mat4 perspective(float fov_y_radians, float aspect, float near_plane, float far_plane);
    static Mat4 translation(Vec3 value);
    static Mat4 rotation_y(float radians);
};

Mat4 multiply(const Mat4& a, const Mat4& b);

} // namespace gta4web::math
