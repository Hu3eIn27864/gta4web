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
    result.throttle = std::max(throttle_axis_, forward_ ? 1.0f : 0.0f);
    result.brake = std::max(brake_axis_, (brake_key_ || reverse_) ? 1.0f : 0.0f);
    result.steering = steering_axis_;
    if (left_) result.steering -= 1.0f;
    if (right_) result.steering += 1.0f;
    result.steering = clamp_axis(result.steering);
    result.handbrake = std::max(handbrake_axis_, handbrake_key_ ? 1.0f : 0.0f);
    return result;
}

} // namespace gta4web::vehicles
