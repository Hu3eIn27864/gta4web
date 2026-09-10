#pragma once

#include "../renderer/math.hpp"

#include <cstdint>

namespace gta4web::world {

struct SectorCoord {
    std::int32_t x = 0;
    std::int32_t z = 0;

    friend constexpr bool operator==(SectorCoord a, SectorCoord b) {
        return a.x == b.x && a.z == b.z;
    }
};

class SectorGrid {
public:
    explicit SectorGrid(float sector_size = 128.0f) : sector_size_(sector_size) {}

    SectorCoord locate(const renderer::math::Vec3& position) const;
    float sector_size() const { return sector_size_; }

private:
    float sector_size_;
};

} // namespace gta4web::world
