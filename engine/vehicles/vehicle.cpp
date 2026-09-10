#include "vehicle.hpp"

#include <algorithm>
#include <cmath>

namespace gta4web::vehicles {
namespace {
constexpr float kGravity = 9.81f;
constexpr float kPi = 3.14159265359f;

float clamp_unit(float value) { return std::clamp(value, -1.0f, 1.0f); }
float sign_or_zero(float value) { return value > 0.0f ? 1.0f : (value < 0.0f ? -1.0f : 0.0f); }
}

Vehicle::Vehicle(VehicleConfig config) : config_(config) {
    config_.mass = std::max(config_.mass, 1.0f);
    config_.wheelbase = std::max(config_.wheelbase, 0.1f);
    config_.wheel_radius = std::max(config_.wheel_radius, 0.05f);
    config_.suspension_rest_length = std::max(config_.suspension_rest_length, 0.05f);
    config_.suspension_travel = std::max(config_.suspension_travel, 0.0f);
    config_.suspension_stiffness = std::max(config_.suspension_stiffness, 1.0f);
    config_.suspension_damping = std::max(config_.suspension_damping, 0.0f);
    for (WheelState& wheel : state_.wheels) wheel.driven = true;
}

void Vehicle::update(const VehicleInput& raw_input, float dt) {
    if (dt <= 0.0f) return;

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

    const float wheel_offsets[4][2] = {
        { config_.wheelbase * 0.5f, -config_.track_width * 0.5f },
        { config_.wheelbase * 0.5f,  config_.track_width * 0.5f },
        {-config_.wheelbase * 0.5f, -config_.track_width * 0.5f },
        {-config_.wheelbase * 0.5f,  config_.track_width * 0.5f }
    };

    const float max_suspension = config_.suspension_rest_length + config_.suspension_travel;
    const float static_load = config_.mass * kGravity * 0.25f;
    float total_longitudinal_force = 0.0f;
    float total_lateral_force = 0.0f;
    float yaw_torque = 0.0f;

    for (int i = 0; i < 4; ++i) {
        WheelState& wheel = state_.wheels[i];
        const bool front = i < 2;
        const float steer_angle = front ? steering * config_.max_steer_angle : 0.0f;
        wheel.steer_angle = steer_angle;

        const float local_x = wheel_offsets[i][0];
        const float local_z = wheel_offsets[i][1];
        const float wheel_world_x = state_.position.x + local_x * forward_x + local_z * right_x;
        const float wheel_world_z = state_.position.z + local_x * forward_z + local_z * right_z;
        (void)wheel_world_x;
        (void)wheel_world_z;

        // Raycast against the flat prototype ground. The suspension is a spring-damper
        // rather than a hard chassis constraint, so body height and load change smoothly.
        const float wheel_center_y = state_.position.y + 0.20f;
        const float ray_length = max_suspension + config_.wheel_radius;
        const float ground_distance = wheel_center_y - config_.wheel_radius;
        const float target_length = std::clamp(ground_distance, 0.0f, max_suspension);
        wheel.grounded = ground_distance <= ray_length;
        wheel.suspension_velocity = (wheel.suspension_length - target_length) / dt;
        wheel.suspension_length = target_length;

        if (!wheel.grounded) {
            wheel.normal_load = 0.0f;
            continue;
        }

        const float compression = std::max(0.0f, config_.suspension_rest_length - wheel.suspension_length);
        const float spring_force = compression * config_.suspension_stiffness;
        const float damper_force = wheel.suspension_velocity * config_.suspension_damping;
        wheel.normal_load = std::max(0.0f, static_load + spring_force + damper_force);

        const float wheel_forward_x = front ? std::sin(state_.yaw + steer_angle) : forward_x;
        const float wheel_forward_z = front ? std::cos(state_.yaw + steer_angle) : forward_z;
        const float wheel_right_x = std::cos(state_.yaw + steer_angle);
        const float wheel_right_z = -std::sin(state_.yaw + steer_angle);

        const float local_forward_speed = state_.velocity.x * wheel_forward_x + state_.velocity.z * wheel_forward_z;
        const float local_lateral_speed = state_.velocity.x * wheel_right_x + state_.velocity.z * wheel_right_z;
        const float wheel_angular_speed = wheel.angular_speed;
        const float wheel_surface_speed = wheel_angular_speed * config_.wheel_radius;

        wheel.slip_ratio = (std::abs(local_forward_speed) > 0.5f)
            ? (wheel_surface_speed - local_forward_speed) / std::max(std::abs(local_forward_speed), 0.5f)
            : 0.0f;
        wheel.slip_angle = std::atan2(local_lateral_speed, std::max(std::abs(local_forward_speed), 0.5f));

        float drive_force = wheel.driven ? throttle * config_.engine_force * 0.25f : 0.0f;
        const float brake_force = brake * config_.brake_force * 0.25f;
        drive_force -= sign_or_zero(local_forward_speed) * brake_force;
        drive_force -= local_forward_speed * config_.drivetrain_drag * 0.25f;

        const float max_tire_force = wheel.normal_load * config_.tire_longitudinal_grip;
        const float longitudinal_force = std::clamp(drive_force, -max_tire_force, max_tire_force);
        const float grip = config_.tire_lateral_grip * (1.0f - (front ? 0.0f : 0.25f) - 0.7f * handbrake);
        const float lateral_force = std::clamp(-local_lateral_speed * grip * config_.mass * 0.25f,
                                               -wheel.normal_load * config_.tire_lateral_grip,
                                               wheel.normal_load * config_.tire_lateral_grip);

        total_longitudinal_force += longitudinal_force * wheel_forward_x + lateral_force * wheel_right_x;
        total_lateral_force += longitudinal_force * wheel_forward_z + lateral_force * wheel_right_z;

        const float force_x = longitudinal_force * wheel_forward_x + lateral_force * wheel_right_x;
        const float force_z = longitudinal_force * wheel_forward_z + lateral_force * wheel_right_z;
        yaw_torque += local_x * force_z - local_z * force_x;

        const float inertia_blend = std::clamp(12.0f * dt, 0.0f, 1.0f);
        const float target_wheel_speed = local_forward_speed / config_.wheel_radius;
        wheel.angular_speed += (target_wheel_speed - wheel.angular_speed) * inertia_blend;
    }

    const float acceleration_x = total_longitudinal_force / config_.mass;
    const float acceleration_z = total_lateral_force / config_.mass;
    state_.velocity.x += acceleration_x * dt;
    state_.velocity.z += acceleration_z * dt;

    // Aerodynamic/rolling drag keeps the car from retaining speed forever.
    const float drag = 0.15f * state_.speed * state_.speed + config_.rolling_resistance * state_.speed;
    if (state_.speed > 0.01f) {
        const float drag_accel = drag / config_.mass;
        state_.velocity.x -= state_.velocity.x / state_.speed * drag_accel * dt;
        state_.velocity.z -= state_.velocity.z / state_.speed * drag_accel * dt;
    }

    const float speed = std::sqrt(state_.velocity.x * state_.velocity.x + state_.velocity.z * state_.velocity.z);
    if (speed > config_.max_speed) {
        const float scale = config_.max_speed / speed;
        state_.velocity.x *= scale;
        state_.velocity.z *= scale;
    }
    state_.speed = std::sqrt(state_.velocity.x * state_.velocity.x + state_.velocity.z * state_.velocity.z);

    const float yaw_inertia = config_.mass * config_.wheelbase * config_.wheelbase * 0.12f;
    const float yaw_accel = yaw_torque / std::max(yaw_inertia, 1.0f);
    state_.yaw_rate += yaw_accel * dt;
    state_.yaw_rate *= std::max(0.0f, 1.0f - 1.8f * dt);
    state_.yaw += state_.yaw_rate * dt;

    state_.position.x += state_.velocity.x * dt;
    state_.position.z += state_.velocity.z * dt;

    // Keep the chassis above the ground plane using the wheel/suspension envelope.
    float average_length = 0.0f;
    int grounded_wheels = 0;
    for (const WheelState& wheel : state_.wheels) {
        if (wheel.grounded) { average_length += wheel.suspension_length; ++grounded_wheels; }
    }
    if (grounded_wheels > 0) {
        average_length /= static_cast<float>(grounded_wheels);
        state_.position.y = 0.20f + config_.wheel_radius + (average_length - config_.suspension_rest_length) * 0.35f;
        if (state_.position.y < 0.0f) state_.position.y = 0.0f;
    }
}

} // namespace gta4web::vehicles
