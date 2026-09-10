#include "physics_world.hpp"

namespace gta4web::physics {

PhysicsBody& PhysicsWorld::create_body() {
    bodies_.push_back({});
    return bodies_.back();
}

void PhysicsWorld::clear() {
    bodies_.clear();
}

void PhysicsWorld::step(float dt) {
    if (dt <= 0.0f) {
        return;
    }

    for (PhysicsBody& body : bodies_) {
        body.rigid_body.integrate(body.position, dt);

        const Contact ground = ground_contact(
            make_aabb(body.position, body.half_extents));
        if (ground.hit && body.rigid_body.velocity.y < 0.0f) {
            body.position.y += ground.penetration;
            body.rigid_body.velocity.y = 0.0f;
        }
    }
}

} // namespace gta4web::physics
