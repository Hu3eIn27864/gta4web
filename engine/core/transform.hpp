#pragma once

#include "../renderer/math.hpp"

namespace gta4web::core {

struct Transform {
    renderer::math::Vec3 position{};
    renderer::math::Vec3 rotation{};
    renderer::math::Vec3 scale{1.0f, 1.0f, 1.0f};
};

} // namespace gta4web::core
