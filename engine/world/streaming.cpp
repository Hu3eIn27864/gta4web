#include "streaming.hpp"

#include <algorithm>

namespace gta4web::world {

void StreamingGrid::update(SectorCoord center) {
    center_ = center;
    loaded_.clear();

    const int radius = std::max(radius_, 0);
    for (int z = -radius; z <= radius; ++z) {
        for (int x = -radius; x <= radius; ++x) {
            loaded_.insert({center.x + x, center.z + z});
        }
    }
}

bool StreamingGrid::is_loaded(SectorCoord coord) const {
    return loaded_.find(coord) != loaded_.end();
}

} // namespace gta4web::world
