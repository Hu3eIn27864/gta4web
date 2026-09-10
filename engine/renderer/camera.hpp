#pragma once

#include "math.hpp"

namespace gta4web::renderer {

class Camera {
public:
    void set_aspect(float aspect) { aspect_ = aspect; }
    void set_fov(float radians) { fov_y_radians_ = radians; }
    void set_clip_planes(float near_plane, float far_plane);

    math::Mat4 projection() const;
    math::Mat4 view() const;
    math::Mat4 view_projection() const;
    const math::Vec3& position() const { return position_; }
    void set_position(math::Vec3 position) { position_ = position; }
    void set_target(math::Vec3 target) { target_ = target; }
    void set_up(math::Vec3 up) { up_ = up; }

private:
    math::Vec3 position_{0.0f, 2.5f, -7.0f};
    math::Vec3 target_{0.0f, 0.8f, 0.0f};
    math::Vec3 up_{0.0f, 1.0f, 0.0f};
    float aspect_ = 16.0f / 9.0f;
    float fov_y_radians_ = 1.04719755f;
    float near_plane_ = 0.1f;
    float far_plane_ = 2000.0f;
};

} // namespace gta4web::renderer
