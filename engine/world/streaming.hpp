#pragma once

#include "sector.hpp"

#include <unordered_set>

namespace gta4web::world {

struct SectorCoordHash {
    std::size_t operator()(SectorCoord value) const noexcept {
        const auto x = static_cast<std::uint64_t>(static_cast<std::uint32_t>(value.x));
        const auto z = static_cast<std::uint64_t>(static_cast<std::uint32_t>(value.z));
        return static_cast<std::size_t>((x * 0x9E3779B185EBCA87ULL) ^ (z + 0x9E3779B97F4A7C15ULL + (x << 6) + (x >> 2)));
    }
};

class StreamingGrid {
public:
    explicit StreamingGrid(int radius = 2) : radius_(radius) {}

    void update(SectorCoord center);
    bool is_loaded(SectorCoord coord) const;
    const std::unordered_set<SectorCoord, SectorCoordHash>& loaded() const { return loaded_; }

private:
    int radius_;
    SectorCoord center_{};
    std::unordered_set<SectorCoord, SectorCoordHash> loaded_;
};

} // namespace gta4web::world
