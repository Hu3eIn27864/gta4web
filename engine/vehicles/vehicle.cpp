#include "vehicle.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace gta4web::vehicles {
namespace {
constexpr float kGravity = 9.81f;
float clamp_unit(float v) { return std::clamp(v, -1.0f, 1.0f); }
float safe_div(float a, float b) { return std::abs(b) > 0.0001f ? a / b : 0.0f; }
float sign(float v) { return v > 0.0f ? 1.0f : (v < 0.0f ? -1.0f : 0.0f); }
}

Vehicle::Vehicle(VehicleConfig config) : config_(config) {
    config_.mass = std::max(config_.mass, 1.0f);
    config_.wheelbase = std::max(config_.wheelbase, 0.1f);
    config_.track_width = std::max(config_.track_width, 0.1f);
    config_.wheel_radius = std::max(config_.wheel_radius, 0.05f);
    config_.suspension_rest_length = std::max(config_.suspension_rest_length, 0.05f);
    config_.suspension_travel = std::max(config_.suspension_travel, 0.01f);
    for (WheelState& wheel : state_.wheels) {
        wheel.driven = true;
        wheel.grounded = false;
        wheel.suspension_length = config_.suspension_rest_length;
    }
}

void Vehicle::update(const VehicleInput& raw, float dt) {
    if (dt <= 0.0f) return;
    const float throttle = std::clamp(raw.throttle, 0.0f, 1.0f);
    const float brake = std::clamp(raw.brake, 0.0f, 1.0f);
    const float steering = clamp_unit(raw.steering);
    const float handbrake = std::clamp(raw.handbrake, 0.0f, 1.0f);

    const float cy = std::cos(state_.yaw), sy = std::sin(state_.yaw);
    const math::Vec3 forward{sy, 0.0f, cy};
    const math::Vec3 right{cy, 0.0f, -sy};
    const float half_wb = config_.wheelbase * 0.5f;
    const float half_track = config_.track_width * 0.5f;
    const std::array<math::Vec3, 4> local = {
        math::Vec3{-half_track, 0.0f, half_wb}, math::Vec3{half_track, 0.0f, half_wb},
        math::Vec3{-half_track, 0.0f, -half_wb}, math::Vec3{half_track, 0.0f, -half_wb}
    };

    const float cp = std::cos(state_.pitch), sp = std::sin(state_.pitch);
    const float cr = std::cos(state_.roll), sr = std::sin(state_.roll);
    auto rotate_body = [&](math::Vec3 p) {
        math::Vec3 r{p.x, p.y * cr - p.z * sr, p.y * sr + p.z * cr};
        return math::Vec3{r.x * cp + r.z * sp, r.y, -r.x * sp + r.z * cp};
    };

    std::array<float, 4> compression{};
    std::array<float, 4> load{};
    const float static_load = config_.mass * kGravity * 0.25f;
    const float max_len = config_.suspension_rest_length + config_.suspension_travel;

    // Four wheel colliders: each wheel casts a suspension ray to the ground plane.
    for (int i = 0; i < 4; ++i) {
        WheelState& wheel = state_.wheels[i];
        const math::Vec3 offset = rotate_body(local[i]);
        const math::Vec3 wheel_position = state_.position + right * offset.x + forward * offset.z + math::Vec3{0.0f, offset.y, 0.0f};
        wheel.contact_position = wheel_position;
        const float distance = wheel_position.y - config_.wheel_radius;
        const float target = std::clamp(distance, 0.0f, max_len);
        wheel.suspension_velocity = safe_div(target - wheel.suspension_length, dt);
        wheel.suspension_length = target;
        wheel.grounded = distance <= max_len;
        compression[i] = std::max(0.0f, config_.suspension_rest_length - target);
        load[i] = wheel.grounded
            ? std::max(0.0f, static_load + compression[i] * config_.suspension_stiffness - wheel.suspension_velocity * config_.suspension_damping)
            : 0.0f;
    }

    // Anti-roll bars transfer load across each axle when left/right compression differs.
    for (int axle = 0; axle < 2; ++axle) {
        const int l = axle * 2, r = l + 1;
        const float k = axle == 0 ? config_.anti_roll_front : config_.anti_roll_rear;
        const float transfer = (compression[l] - compression[r]) * k;
        load[l] = std::max(0.0f, load[l] - transfer);
        load[r] = std::max(0.0f, load[r] + transfer);
    }

    // Longitudinal load transfer feeds the chassis pitch response.
    const float forward_speed = math::dot(state_.velocity, forward);
    const float drive_accel = throttle * config_.engine_force / config_.mass;
    const float brake_accel = brake * config_.brake_force / config_.mass * sign(forward_speed);
    const float transfer = (drive_accel - brake_accel) * config_.mass * 0.42f / config_.wheelbase;
    for (int i = 0; i < 2; ++i) load[i] = std::max(0.0f, load[i] - transfer * 0.5f);
    for (int i = 2; i < 4; ++i) load[i] = std::max(0.0f, load[i] + transfer * 0.5f);

    math::Vec3 total_force{};
    float yaw_torque = 0.0f;
    const float axle_drive[2] = {throttle * config_.engine_force * 0.5f, throttle * config_.engine_force * 0.5f};
    const float brake_force = brake * config_.brake_force;
    const float lock = std::clamp(config_.differential_lock, 0.0f, 1.0f);

    for (int axle = 0; axle < 2; ++axle) {
        const int l = axle * 2, r = l + 1;
        const float ll = std::max(load[l], 1.0f), rl = std::max(load[r], 1.0f);
        const float sum = ll + rl;
        const float base_l = axle_drive[axle] * ll / sum;
        const float base_r = axle_drive[axle] * rl / sum;
        const float avg = axle_drive[axle] * 0.5f;
        const float drive[2] = {base_l * (1.0f - lock) + avg * lock, base_r * (1.0f - lock) + avg * lock};

        for (int side = 0; side < 2; ++side) {
            const int i = axle * 2 + side;
            WheelState& wheel = state_.wheels[i];
            if (!wheel.grounded) continue;
            const float steer_angle = axle == 0 ? steering * config_.max_steer_angle : 0.0f;
            wheel.steer_angle = steer_angle;
            const float wa = state_.yaw + steer_angle;
            const math::Vec3 wf{std::sin(wa), 0.0f, std::cos(wa)};
            const math::Vec3 wr{std::cos(wa), 0.0f, -std::sin(wa)};
            const math::Vec3 rpos = right * local[i].x + forward * local[i].z;
            const math::Vec3 point_velocity = state_.velocity + math::Vec3{-state_.yaw_rate * rpos.z, 0.0f, state_.yaw_rate * rpos.x};
            const float vlong = math::dot(point_velocity, wf);
            const float vlat = math::dot(point_velocity, wr);
            const float surface_speed = wheel.angular_speed * config_.wheel_radius;
            wheel.slip_ratio = (surface_speed - vlong) / std::max(std::abs(vlong), 2.0f);
            wheel.slip_angle = std::atan2(vlat, std::abs(vlong) + 0.5f);

            float fx = drive[side] - brake_force * 0.25f * sign(vlong);
            if (axle == 1) fx -= handbrake * brake_force * 0.20f * sign(vlong);
            const float normal = std::max(load[i], 50.0f);
            fx = std::clamp(fx, -normal * config_.tire_longitudinal_grip, normal * config_.tire_longitudinal_grip);
            const float rear_grip = axle == 1 ? (1.0f - 0.65f * handbrake) : 1.0f;
            const float fy = std::clamp(-vlat * config_.lateral_grip * config_.mass * 0.25f,
                -normal * config_.tire_lateral_grip * rear_grip, normal * config_.tire_lateral_grip * rear_grip);
            const math::Vec3 force = wf * fx + wr * fy;
            total_force += force;
            yaw_torque += rpos.x * force.z - rpos.z * force.x;

            const float inertia = 1.4f * config_.wheel_radius * config_.wheel_radius;
            const float wheel_torque = drive[side] * config_.wheel_radius - fx * config_.wheel_radius - wheel.angular_speed * config_.drivetrain_drag;
            wheel.angular_speed += safe_div(wheel_torque, inertia) * dt;
        }
    }

    const float planar_speed = std::sqrt(state_.velocity.x * state_.velocity.x + state_.velocity.z * state_.velocity.z);
    total_force -= state_.velocity * (config_.rolling_resistance + 0.04f * planar_speed);
    state_.velocity += (total_force / config_.mass) * dt;
    state_.speed = std::sqrt(state_.velocity.x * state_.velocity.x + state_.velocity.z * state_.velocity.z);
    if (state_.speed > config_.max_speed) {
        const float scale = config_.max_speed / state_.speed;
        state_.velocity.x *= scale;
        state_.velocity.z *= scale;
        state_.speed = config_.max_speed;
    }

    const float yaw_inertia = config_.mass * (config_.wheelbase * config_.wheelbase + config_.track_width * config_.track_width) / 12.0f;
    state_.yaw_rate += yaw_torque / std::max(yaw_inertia, 1.0f) * dt;
    state_.yaw_rate *= std::max(0.0f, 1.0f - 1.6f * dt);
    state_.yaw += state_.yaw_rate * dt;
    state_.position += state_.velocity * dt;

    // Pitch/roll are real state variables driven by front/rear and left/right wheel loads.
    const float front_load = load[0] + load[1], rear_load = load[2] + load[3];
    const float left_load = load[0] + load[2], right_load = load[1] + load[3];
    const float target_pitch = std::clamp((rear_load - front_load) / (config_.mass * kGravity) * 0.28f, -0.30f, 0.30f);
    const float target_roll = std::clamp((right_load - left_load) / (config_.mass * kGravity) * 0.35f, -0.35f, 0.35f);
    state_.pitch_rate += (target_pitch - state_.pitch) * 18.0f * dt;
    state_.roll_rate += (target_roll - state_.roll) * 22.0f * dt;
    state_.pitch_rate *= std::max(0.0f, 1.0f - 5.0f * dt);
    state_.roll_rate *= std::max(0.0f, 1.0f - 5.0f * dt);
    state_.pitch += state_.pitch_rate * dt;
    state_.roll += state_.roll_rate * dt;
    state_.pitch = std::clamp(state_.pitch, -0.35f, 0.35f);
    state_.roll = std::clamp(state_.roll, -0.35f, 0.35f);

    for (int i = 0; i < 4; ++i) {
        state_.wheels[i].normal_load = load[i];
        state_.wheels[i].suspension_length = std::clamp(state_.wheels[i].suspension_length,
            config_.suspension_rest_length - config_.suspension_travel, max_len);
    }
}

} // namespace gta4web::vehicles
