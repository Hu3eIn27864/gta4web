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

} // namespace gta4web::renderer
