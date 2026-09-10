#pragma once

#include "collision.hpp"
#include "rigid_body.hpp"

#include <vector>

namespace gta4web::physics {

struct PhysicsBody {
    math::Vec3 position{};
    math::Vec3 half_extents{0.5f, 0.5f, 0.5f};
    RigidBody rigid_body{};
};

class PhysicsWorld {
public:
    PhysicsBody& create_body();
    void clear();
    void step(float dt);

    const std::vector<PhysicsBody>& bodies() const { return bodies_; }
    std::vector<PhysicsBody>& bodies() { return bodies_; }

private:
    std::vector<PhysicsBody> bodies_;
};

} // namespace gta4web::physics
