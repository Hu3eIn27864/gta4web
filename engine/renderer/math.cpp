#include "math.hpp"

#include <algorithm>

namespace gta4web::math {

Mat4 Mat4::identity() {
    Mat4 result{};
    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::perspective(float fov_y_radians, float aspect, float near_plane, float far_plane) {
    const float safe_aspect = std::max(aspect, 0.001f);
    const float f = 1.0f / std::tan(fov_y_radians * 0.5f);

    Mat4 result{};
    result.m[0] = f / safe_aspect;
    result.m[5] = f;
    result.m[10] = (far_plane + near_plane) / (near_plane - far_plane);
    result.m[11] = -1.0f;
    result.m[14] = (2.0f * far_plane * near_plane) / (near_plane - far_plane);
    return result;
}

Mat4 Mat4::translation(Vec3 value) {
    Mat4 result = identity();
    result.m[12] = value.x;
    result.m[13] = value.y;
    result.m[14] = value.z;
    return result;
}

Mat4 Mat4::rotation_y(float radians) {
    const float c = std::cos(radians);
    const float s = std::sin(radians);

    Mat4 result = identity();
    result.m[0] = c;
    result.m[2] = -s;
    result.m[8] = s;
    result.m[10] = c;
    return result;
}

Mat4 multiply(const Mat4& a, const Mat4& b) {
    Mat4 result{};
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            float value = 0.0f;
            for (int k = 0; k < 4; ++k) {
                value += a.m[k * 4 + row] * b.m[column * 4 + k];
            }
            result.m[column * 4 + row] = value;
        }
    }
    return result;
}

} // namespace gta4web::math
