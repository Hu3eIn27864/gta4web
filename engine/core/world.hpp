#pragma once

#include "entity.hpp"
#include "transform.hpp"

#include <cstddef>
#include <unordered_map>

namespace gta4web::core {

class World {
public:
    EntityId create_entity();
    bool destroy_entity(EntityId entity);
    bool exists(EntityId entity) const;

    Transform* transform(EntityId entity);
    const Transform* transform(EntityId entity) const;

    std::size_t entity_count() const { return transforms_.size(); }
    void clear();

private:
    EntityId next_entity_ = 1;
    std::unordered_map<EntityId, Transform> transforms_;
};

} // namespace gta4web::core
