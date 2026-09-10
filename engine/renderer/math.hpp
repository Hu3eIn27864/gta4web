#pragma once

#include <cmath>

namespace gta4web::math {

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct Mat4 {
    float m[16]{};

    static Mat4 identity();
    static Mat4 perspective(float fov_y_radians, float aspect, float near_plane, float far_plane);
    static Mat4 translation(Vec3 value);
    static Mat4 rotation_y(float radians);
};

Mat4 multiply(const Mat4& a, const Mat4& b);

} // namespace gta4web::math
