#pragma once

#include "../renderer/math.hpp"

namespace gta4web::core {

struct Transform {
    math::Vec3 position{};
    math::Vec3 rotation{};
    math::Vec3 scale{1.0f, 1.0f, 1.0f};
};

} // namespace gta4web::core
