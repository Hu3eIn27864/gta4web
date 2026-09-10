#include "input.hpp"

#include <algorithm>

namespace gta4web::vehicles {
namespace {
float clamp_axis(float value) { return std::clamp(value, -1.0f, 1.0f); }
}

void VehicleInputState::set_throttle(float value) { throttle_axis_ = clamp_axis(value); }
void VehicleInputState::set_brake(float value) { brake_axis_ = clamp_axis(value); }
void VehicleInputState::set_steering(float value) { steering_axis_ = clamp_axis(value); }
void VehicleInputState::set_handbrake(float value) { handbrake_axis_ = clamp_axis(value); }

void VehicleInputState::press_forward(bool pressed) { forward_ = pressed; }
void VehicleInputState::press_reverse(bool pressed) { reverse_ = pressed; }
void VehicleInputState::press_left(bool pressed) { left_ = pressed; }
void VehicleInputState::press_right(bool pressed) { right_ = pressed; }
void VehicleInputState::press_brake_key(bool pressed) { brake_key_ = pressed; }
void VehicleInputState::press_handbrake(bool pressed) { handbrake_key_ = pressed; }

VehicleInput VehicleInputState::sample() const {
    VehicleInput result;
    result.throttle = throttle_axis_;

    // Keyboard direction has priority over the analog throttle. The vehicle
    // resolves the direction-change interlock against its current velocity.
    if (forward_ && !reverse_) {
        result.throttle = 1.0f;
    } else if (reverse_ && !forward_) {
        result.throttle = -1.0f;
    }

    result.brake = std::max(0.0f, brake_axis_);
    if (brake_key_) result.brake = 1.0f;

    result.steering = steering_axis_;
    if (left_) result.steering -= 1.0f;
    if (right_) result.steering += 1.0f;
    result.steering = clamp_axis(result.steering);
    result.handbrake = std::max(handbrake_axis_, handbrake_key_ ? 1.0f : 0.0f);
    return result;
}

} // namespace gta4web::vehicles
