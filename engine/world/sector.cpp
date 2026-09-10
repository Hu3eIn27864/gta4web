#include "sector.hpp"

#include <cmath>

namespace gta4web::world {

SectorCoord SectorGrid::locate(const math::Vec3& position) const {
    return {
        static_cast<std::int32_t>(std::floor(position.x / sector_size_)),
        static_cast<std::int32_t>(std::floor(position.z / sector_size_))
    };
}

} // namespace gta4web::world
