#include "rigid_body.hpp"

#include <algorithm>

namespace gta4web::physics {

void RigidBody::set_mass(float value) {
    mass = std::max(value, 0.001f);
    inv_mass = dynamic ? 1.0f / mass : 0.0f;
}

void RigidBody::add_force(math::Vec3 value) {
    if (dynamic) {
        force.x += value.x;
        force.y += value.y;
        force.z += value.z;
    }
}

void RigidBody::clear_forces() {
    force = {};
}

void RigidBody::integrate(math::Vec3& position, float dt) {
    if (!dynamic || dt <= 0.0f) {
        clear_forces();
        return;
    }

    math::Vec3 acceleration{
        force.x * inv_mass,
        force.y * inv_mass - (use_gravity ? 9.81f * gravity_scale : 0.0f),
        force.z * inv_mass
    };

    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;
    velocity.z += acceleration.z * dt;

    const float damping = std::max(0.0f, 1.0f - linear_damping * dt);
    velocity.x *= damping;
    velocity.y *= damping;
    velocity.z *= damping;

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
    position.z += velocity.z * dt;
    clear_forces();
}

} // namespace gta4web::physics
