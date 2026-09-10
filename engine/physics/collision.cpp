#include "collision.hpp"

#include <algorithm>

namespace gta4web::physics {

Aabb make_aabb(math::Vec3 center, math::Vec3 half_extents) {
    return {
        {center.x - half_extents.x, center.y - half_extents.y, center.z - half_extents.z},
        {center.x + half_extents.x, center.y + half_extents.y, center.z + half_extents.z}
    };
}

Contact intersect_aabb(const Aabb& a, const Aabb& b) {
    const float overlap_x = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    const float overlap_y = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    const float overlap_z = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

    if (overlap_x <= 0.0f || overlap_y <= 0.0f || overlap_z <= 0.0f) {
        return {};
    }

    Contact contact{true, {}, overlap_x};
    const math::Vec3 delta{
        (a.min.x + a.max.x) - (b.min.x + b.max.x),
        (a.min.y + a.max.y) - (b.min.y + b.max.y),
        (a.min.z + a.max.z) - (b.min.z + b.max.z)
    };

    contact.normal = {delta.x < 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f};
    if (overlap_y < contact.penetration) {
        contact.penetration = overlap_y;
        contact.normal = {0.0f, delta.y < 0.0f ? -1.0f : 1.0f, 0.0f};
    }
    if (overlap_z < contact.penetration) {
        contact.penetration = overlap_z;
        contact.normal = {0.0f, 0.0f, delta.z < 0.0f ? -1.0f : 1.0f};
    }
    return contact;
}

Contact ground_contact(const Aabb& box, float ground_y) {
    const float penetration = ground_y - box.min.y;
    if (penetration <= 0.0f) {
        return {};
    }
    return {true, {0.0f, 1.0f, 0.0f}, penetration};
}

} // namespace gta4web::physics
