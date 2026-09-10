#include "world.hpp"

namespace gta4web::core {

EntityId World::create_entity() {
    // Keep zero reserved for an invalid handle. Avoid wrapping back to it.
    if (next_entity_ == InvalidEntity) {
        next_entity_ = 1;
    }

    const EntityId entity = next_entity_++;
    transforms_.emplace(entity, Transform{});
    return entity;
}

bool World::destroy_entity(EntityId entity) {
    if (entity == InvalidEntity) {
        return false;
    }
    return transforms_.erase(entity) != 0;
}

bool World::exists(EntityId entity) const {
    return entity != InvalidEntity && transforms_.find(entity) != transforms_.end();
}

Transform* World::transform(EntityId entity) {
    const auto it = transforms_.find(entity);
    return it == transforms_.end() ? nullptr : &it->second;
}

const Transform* World::transform(EntityId entity) const {
    const auto it = transforms_.find(entity);
    return it == transforms_.end() ? nullptr : &it->second;
}

void World::clear() {
    transforms_.clear();
    next_entity_ = 1;
}

} // namespace gta4web::core
