#pragma once

#include "vehicle.hpp"

namespace gta4web::vehicles {

class VehicleInputState {
public:
    void set_throttle(float value);
    void set_brake(float value);
    void set_steering(float value);
    void set_handbrake(float value);

    void press_forward(bool pressed);
    void press_reverse(bool pressed);
    void press_left(bool pressed);
    void press_right(bool pressed);
    void press_brake_key(bool pressed);
    void press_handbrake(bool pressed);

    VehicleInput sample() const;

private:
    float throttle_axis_ = 0.0f;
    float brake_axis_ = 0.0f;
    float steering_axis_ = 0.0f;
    float handbrake_axis_ = 0.0f;
    bool forward_ = false;
    bool reverse_ = false;
    bool left_ = false;
    bool right_ = false;
    bool brake_key_ = false;
    bool handbrake_key_ = false;
};

} // namespace gta4web::vehicles
