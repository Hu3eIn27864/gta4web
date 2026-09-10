#include "camera.hpp"

#include <algorithm>

namespace gta4web::renderer {

void Camera::set_clip_planes(float near_plane, float far_plane) {
    near_plane_ = std::max(near_plane, 0.001f);
    far_plane_ = std::max(far_plane, near_plane_ + 0.001f);
}

math::Mat4 Camera::projection() const {
    return math::Mat4::perspective(fov_y_radians_, aspect_, near_plane_, far_plane_);
}

math::Mat4 Camera::view() const {
    return math::Mat4::look_at(position_, target_, up_);
}

math::Mat4 Camera::view_projection() const {
    return math::multiply(projection(), view());
}

} // namespace gta4web::renderer
