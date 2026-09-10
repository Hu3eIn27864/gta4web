#include "vehicle.hpp"

#include <algorithm>
#include <cmath>

namespace gta4web::vehicles {
namespace {

float clamp_unit(float value) {
    return std::clamp(value, -1.0f, 1.0f);
}

} // namespace

Vehicle::Vehicle(VehicleConfig config) : config_(config) {
    config_.mass = std::max(config_.mass, 1.0f);
    config_.wheelbase = std::max(config_.wheelbase, 0.1f);
    config_.wheel_radius = std::max(config_.wheel_radius, 0.05f);
    state_.wheels[0].driven = true;
    state_.wheels[1].driven = true;
    state_.wheels[2].driven = true;
    state_.wheels[3].driven = true;
}

void Vehicle::update(const VehicleInput& raw_input, float dt) {
    if (dt <= 0.0f) {
        return;
    }

    const float throttle = std::clamp(raw_input.throttle, 0.0f, 1.0f);
    const float brake = std::clamp(raw_input.brake, 0.0f, 1.0f);
    const float steering = clamp_unit(raw_input.steering);
    const float handbrake = std::clamp(raw_input.handbrake, 0.0f, 1.0f);

    const float forward_x = std::sin(state_.yaw);
    const float forward_z = std::cos(state_.yaw);
    const float right_x = std::cos(state_.yaw);
    const float right_z = -std::sin(state_.yaw);

    const float forward_speed = state_.velocity.x * forward_x + state_.velocity.z * forward_z;
    const float lateral_speed = state_.velocity.x * right_x + state_.velocity.z * right_z;

    float longitudinal_force = throttle * config_.engine_force;
    if (brake > 0.0f) {
        const float brake_direction = forward_speed >= 0.0f ? -1.0f : 1.0f;
        longitudinal_force += brake_direction * brake * config_.brake_force;
    }
    longitudinal_force -= forward_speed * config_.rolling_resistance;

    const float grip = config_.lateral_grip * (1.0f - 0.65f * handbrake);
    const float lateral_force = -lateral_speed * grip * config_.mass;
    const float acceleration_forward = longitudinal_force / config_.mass;
    const float acceleration_lateral = lateral_force / config_.mass;

    state_.velocity.x += (forward_x * acceleration_forward + right_x * acceleration_lateral) * dt;
    state_.velocity.z += (forward_z * acceleration_forward + right_z * acceleration_lateral) * dt;

    const float speed = std::sqrt(state_.velocity.x * state_.velocity.x + state_.velocity.z * state_.velocity.z);
    if (speed > config_.max_speed) {
        const float scale = config_.max_speed / speed;
        state_.velocity.x *= scale;
        state_.velocity.z *= scale;
    }

    state_.speed = std::sqrt(state_.velocity.x * state_.velocity.x + state_.velocity.z * state_.velocity.z);

    const float steer_angle = steering * config_.max_steer_angle;
    const float speed_factor = std::min(std::abs(forward_speed) / 5.0f, 1.0f);
    const float target_yaw_rate = (forward_speed / config_.wheelbase) * std::tan(steer_angle);
    const float yaw_response = std::clamp(8.0f * dt * speed_factor, 0.0f, 1.0f);
    state_.yaw_rate += (target_yaw_rate - state_.yaw_rate) * yaw_response;
    state_.yaw += state_.yaw_rate * dt;

    state_.position.x += state_.velocity.x * dt;
    state_.position.z += state_.velocity.z * dt;

    for (int i = 0; i < 4; ++i) {
        const bool front = i < 2;
        state_.wheels[i].steer_angle = front ? steer_angle : 0.0f;
        state_.wheels[i].angular_speed = forward_speed / config_.wheel_radius;
        state_.wheels[i].grounded = true;
    }
}

} // namespace gta4web::vehicles
