#pragma once

#include "../renderer/math.hpp"

namespace gta4web::physics {

struct RigidBody {
    float mass = 1000.0f;
    float inv_mass = 0.001f;
    math::Vec3 velocity{};
    math::Vec3 force{};
    float gravity_scale = 1.0f;
    float linear_damping = 0.02f;
    bool use_gravity = true;
    bool dynamic = true;

    void set_mass(float value);
    void add_force(math::Vec3 value);
    void clear_forces();
    void integrate(math::Vec3& position, float dt);
};

} // namespace gta4web::physics
