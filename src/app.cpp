#include "gta4web/app.hpp"
#include "camera.hpp"
#include "math.hpp"
#include "core/fixed_timestep.hpp"
#include "physics/collision.hpp"
#include "vehicles/input.hpp"
#include "vehicles/vehicle.hpp"

#include <algorithm>
#include <cmath>

#include <GLES2/gl2.h>
#include <emscripten.h>
#include <emscripten/html5.h>

namespace {
using gta4web::math::Mat4;
using gta4web::math::Vec3;
using gta4web::physics::Aabb;
using gta4web::physics::Contact;
using gta4web::renderer::Camera;
using gta4web::vehicles::Vehicle;
using gta4web::vehicles::VehicleInputState;

GLuint compile_shader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) { glDeleteShader(shader); return 0; }
    return shader;
}

GLuint create_program() {
    static constexpr const char* vertex_source = R"GLSL(
        attribute vec3 a_position;
        uniform mat4 u_mvp;
        void main() { gl_Position = u_mvp * vec4(a_position, 1.0); }
    )GLSL";
    static constexpr const char* fragment_source = R"GLSL(
        precision mediump float;
        uniform vec4 u_color;
        void main() { gl_FragColor = u_color; }
    )GLSL";
    const GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_source);
    const GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (!vs || !fs) return 0;
    const GLuint result = glCreateProgram();
    glAttachShader(result, vs); glAttachShader(result, fs);
    glBindAttribLocation(result, 0, "a_position");
    glLinkProgram(result);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint linked = GL_FALSE;
    glGetProgramiv(result, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) { glDeleteProgram(result); return 0; }
    return result;
}

constexpr float cube_vertices[] = {
    -1,-1,-1, 1,-1,-1, 1,1,-1, -1,-1,-1, 1,1,-1, -1,1,-1,
    -1,-1,1, 1,-1,1, 1,1,1, -1,-1,1, 1,1,1, -1,1,1,
    -1,-1,-1, -1,1,-1, -1,1,1, -1,-1,-1, -1,1,1, -1,-1,1,
    1,-1,-1, 1,1,-1, 1,1,1, 1,-1,-1, 1,1,1, 1,-1,1,
    -1,-1,-1, 1,-1,-1, 1,-1,1, -1,-1,-1, 1,-1,1, -1,-1,1,
    -1,1,-1, 1,1,-1, 1,1,1, -1,1,-1, 1,1,1, -1,1,1
};

GLuint program = 0;
GLint mvp_uniform = -1;
GLint color_uniform = -1;
Camera camera;
Vehicle vehicle;
VehicleInputState input;
gta4web::core::FixedTimestep fixed_step;

constexpr Vec3 vehicle_half_extents{1.05f, 0.50f, 2.00f};
constexpr Aabb collision_blocks[] = {
    {{5.0f, 0.0f, 7.0f}, {8.0f, 1.0f, 8.0f}},
    {{-8.0f, 0.0f, -3.0f}, {-5.0f, 1.0f, 5.0f}},
};

enum KeyCode : int { KeyW = 87, KeyA = 65, KeyS = 83, KeyD = 68, KeySpace = 32 };

EM_BOOL on_key(int event_type, const EmscriptenKeyboardEvent* event, void*) {
    const bool pressed = event_type == EMSCRIPTEN_EVENT_KEYDOWN;
    if (event->repeat && pressed) return EM_TRUE;
    switch (event->keyCode) {
    case KeyW: input.press_forward(pressed); break;
    case KeyA: input.press_left(pressed); break;
    case KeyS: input.press_reverse(pressed); break;
    case KeyD: input.press_right(pressed); break;
    case KeySpace: input.press_handbrake(pressed); break;
    default: return EM_FALSE;
    }
    return EM_TRUE;
}

void resize_canvas() {
    int width = 0, height = 0;
    if (emscripten_get_canvas_element_size("#canvas", &width, &height) == EMSCRIPTEN_RESULT_SUCCESS) {
        glViewport(0, 0, width, height);
        camera.set_aspect(static_cast<float>(width) / static_cast<float>(std::max(height, 1)));
    }
}

Mat4 model_transform(Vec3 position, Vec3 scale, float yaw = 0.0f) {
    return gta4web::math::multiply(
        Mat4::translation(position),
        gta4web::math::multiply(Mat4::rotation_y(yaw), Mat4::scale(scale)));
}

void draw_cube(Vec3 position, Vec3 scale, float r, float g, float b, float yaw = 0.0f) {
    const Mat4 model = model_transform(position, scale, yaw);
    const Mat4 mvp = gta4web::math::multiply(camera.view_projection(), model);
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, mvp.m);
    glUniform4f(color_uniform, r, g, b, 1.0f);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, cube_vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void resolve_vehicle_collisions() {
    auto& state = vehicle.state();
    Aabb car_box = gta4web::physics::make_aabb(state.position, vehicle_half_extents);

    const Contact ground = gta4web::physics::ground_contact(car_box, 0.0f);
    if (ground.hit) {
        state.position += ground.normal * ground.penetration;
        if (state.velocity.y < 0.0f) state.velocity.y = 0.0f;
    }

    for (const Aabb& obstacle : collision_blocks) {
        const Contact contact = gta4web::physics::intersect_aabb(car_box, obstacle);
        if (!contact.hit) continue;
        state.position += contact.normal * contact.penetration;
        const float normal_speed = gta4web::math::dot(state.velocity, contact.normal);
        if (normal_speed < 0.0f) state.velocity -= contact.normal * normal_speed;
        car_box = gta4web::physics::make_aabb(state.position, vehicle_half_extents);
    }
}

void update_camera() {
    const auto& state = vehicle.state();
    const Vec3 forward{std::sin(state.yaw), 0.0f, std::cos(state.yaw)};
    const Vec3 target = state.position + Vec3{0.0f, 0.55f, 0.0f};
    const Vec3 desired_position = target - forward * 7.5f + Vec3{0.0f, 3.1f, 0.0f};
    camera.set_target(target);
    camera.set_position(desired_position);
}

void draw_vehicle() {
    const auto& state = vehicle.state();
    const Vec3 p = state.position;
    const float yaw = state.yaw;

    // Original low-poly test vehicle: chassis, cabin, bumpers, lights and four wheels.
    draw_cube(p + Vec3{0.0f, 0.45f, 0.0f}, {1.05f, 0.45f, 2.0f}, 0.72f, 0.10f, 0.07f, yaw);
    draw_cube(p + Vec3{0.0f, 0.98f, -0.15f}, {0.78f, 0.40f, 0.95f}, 0.10f, 0.14f, 0.18f, yaw);
    draw_cube(p + Vec3{0.0f, 0.35f, 2.03f}, {1.00f, 0.14f, 0.12f}, 0.08f, 0.08f, 0.08f, yaw);
    draw_cube(p + Vec3{0.0f, 0.35f, -2.03f}, {1.00f, 0.14f, 0.12f}, 0.08f, 0.08f, 0.08f, yaw);

    const Vec3 local_wheels[] = {
        {-1.08f, 0.28f, 1.25f}, {1.08f, 0.28f, 1.25f},
        {-1.08f, 0.28f, -1.25f}, {1.08f, 0.28f, -1.25f}
    };
    for (int i = 0; i < 4; ++i) {
        const float c = std::cos(yaw), s = std::sin(yaw);
        const Vec3 l = local_wheels[i];
        const Vec3 world{p.x + l.x * c + l.z * s, p.y + l.y, p.z - l.x * s + l.z * c};
        draw_cube(world, {0.22f, 0.22f, 0.42f}, 0.025f, 0.025f, 0.025f, yaw);
    }

    const Vec3 front_left{p.x - 0.58f * std::cos(yaw) + 1.95f * std::sin(yaw), p.y + 0.55f,
                          p.z + 0.58f * std::sin(yaw) + 1.95f * std::cos(yaw)};
    const Vec3 front_right{p.x + 0.58f * std::cos(yaw) + 1.95f * std::sin(yaw), p.y + 0.55f,
                           p.z - 0.58f * std::sin(yaw) + 1.95f * std::cos(yaw)};
    draw_cube(front_left, {0.18f, 0.12f, 0.10f}, 1.0f, 0.85f, 0.35f, yaw);
    draw_cube(front_right, {0.18f, 0.12f, 0.10f}, 1.0f, 0.85f, 0.35f, yaw);
}

void render_frame() {
    static double last_time = emscripten_get_now() * 0.001;
    const double now = emscripten_get_now() * 0.001;
    const double frame_seconds = std::clamp(now - last_time, 0.0, 0.25);
    last_time = now;

    resize_canvas();
    fixed_step.advance(frame_seconds, [](double step) {
        vehicle.update(input.sample(), static_cast<float>(step));
        resolve_vehicle_collisions();
    });
    update_camera();

    glClearColor(0.025f, 0.035f, 0.055f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    draw_cube({0.0f, -0.10f, 0.0f}, {45.0f, 0.10f, 45.0f}, 0.18f, 0.20f, 0.19f);
    for (const Aabb& block : collision_blocks) {
        const Vec3 center = (block.min + block.max) * 0.5f;
        const Vec3 half = (block.max - block.min) * 0.5f;
        draw_cube(center, half, 0.28f, 0.30f, 0.33f);
    }
    draw_vehicle();
}

} // namespace

namespace gta4web {

bool App::initialize() {
    program = create_program();
    if (!program) return false;
    mvp_uniform = glGetUniformLocation(program, "u_mvp");
    color_uniform = glGetUniformLocation(program, "u_color");
    glEnable(GL_DEPTH_TEST);
    vehicle.state().position = {0.0f, 0.5f, 0.0f};
    vehicle.state().velocity = {};
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, on_key);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, on_key);
    resize_canvas();
    update_camera();
    return true;
}

void App::frame(double) { render_frame(); }

void App::shutdown() {
    if (program) { glDeleteProgram(program); program = 0; }
}

} // namespace gta4web

int main() {
    gta4web::App app;
    if (!app.initialize()) return 1;
    emscripten_set_main_loop([]() { render_frame(); }, 0, EM_TRUE);
    return 0;
}
