#pragma once

#include "../renderer/math.hpp"

namespace gta4web::vehicles {

struct VehicleInput {
    float throttle = 0.0f;
    float brake = 0.0f;
    float steering = 0.0f;
    float handbrake = 0.0f;
};

struct WheelState {
    bool driven = false;
    bool grounded = false;
    float steer_angle = 0.0f;
    float angular_speed = 0.0f;
    float suspension_length = 0.35f;
    float suspension_velocity = 0.0f;
    float normal_load = 0.0f;
    float slip_ratio = 0.0f;
    float slip_angle = 0.0f;
};

struct VehicleConfig {
    float mass = 1400.0f;
    float engine_force = 8500.0f;
    float brake_force = 12000.0f;
    float max_steer_angle = 0.65f;
    float wheel_radius = 0.34f;
    float wheelbase = 2.6f;
    float track_width = 1.6f;
    float max_speed = 65.0f;
    float rolling_resistance = 1.5f;
    float lateral_grip = 7.0f;
    float suspension_rest_length = 0.35f;
    float suspension_travel = 0.18f;
    float suspension_stiffness = 28000.0f;
    float suspension_damping = 4200.0f;
    float tire_longitudinal_grip = 1.15f;
    float tire_lateral_grip = 1.45f;
    float drivetrain_drag = 0.8f;
};

struct VehicleState {
    math::Vec3 position{};
    math::Vec3 velocity{};
    float yaw = 0.0f;
    float yaw_rate = 0.0f;
    float speed = 0.0f;
    WheelState wheels[4]{};
};

class Vehicle {
public:
    explicit Vehicle(VehicleConfig config = {});
    void update(const VehicleInput& input, float dt);
    const VehicleConfig& config() const { return config_; }
    const VehicleState& state() const { return state_; }
    VehicleState& state() { return state_; }
private:
    VehicleConfig config_;
    VehicleState state_;
};

} // namespace gta4web::vehicles
