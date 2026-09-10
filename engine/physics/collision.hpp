#pragma once

#include "../renderer/math.hpp"

namespace gta4web::physics {

struct Aabb {
    math::Vec3 min{};
    math::Vec3 max{};
};

struct Contact {
    bool hit = false;
    math::Vec3 normal{};
    float penetration = 0.0f;
};

Aabb make_aabb(math::Vec3 center, math::Vec3 half_extents);
Contact intersect_aabb(const Aabb& a, const Aabb& b);
Contact ground_contact(const Aabb& box, float ground_y = 0.0f);

} // namespace gta4web::physics
