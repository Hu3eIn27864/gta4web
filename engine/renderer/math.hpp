#pragma once

#include <cmath>

namespace gta4web::math {

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    constexpr Vec3 operator+(Vec3 o) const { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3 operator-(Vec3 o) const { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator/(float s) const { return {x / s, y / s, z / s}; }
    Vec3& operator+=(Vec3 o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(Vec3 o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
};
constexpr Vec3 operator*(float s, Vec3 v) { return v * s; }
inline float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) { return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x}; }
inline float length_squared(Vec3 v) { return dot(v, v); }
inline float length(Vec3 v) { return std::sqrt(length_squared(v)); }
inline Vec3 normalize(Vec3 v) { const float l=length(v); return l>0.00001f ? v/l : Vec3{}; }

struct Mat4 {
    float m[16]{};
    static Mat4 identity();
    static Mat4 perspective(float fov_y_radians, float aspect, float near_plane, float far_plane);
    static Mat4 translation(Vec3 value);
    static Mat4 rotation_x(float radians);
    static Mat4 rotation_y(float radians);
    static Mat4 rotation_z(float radians);
    static Mat4 scale(Vec3 value);
    static Mat4 look_at(Vec3 eye, Vec3 target, Vec3 up);
};
Mat4 multiply(const Mat4& a, const Mat4& b);

} // namespace gta4web::math
