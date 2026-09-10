#include "gta4web/app.hpp"
#include "camera.hpp"
#include "math.hpp"
#include "core/fixed_timestep.hpp"
#include "vehicles/input.hpp"
#include "vehicles/vehicle.hpp"

#include <algorithm>
#include <cmath>

#include <GLES2/gl2.h>
#include <emscripten.h>
#include <emscripten/html5.h>

namespace {
using gta4web::math::Mat4;
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

void draw_cube(const Mat4& model, float r, float g, float b) {
    const Mat4 mvp = gta4web::math::multiply(camera.projection(), model);
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, mvp.m);
    glUniform4f(color_uniform, r, g, b, 1.0f);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, cube_vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void render_frame() {
    static double last_time = emscripten_get_now() * 0.001;
    const double now = emscripten_get_now() * 0.001;
    const double frame_seconds = std::clamp(now - last_time, 0.0, 0.25);
    last_time = now;

    resize_canvas();
    fixed_step.advance(frame_seconds, [](double step) {
        vehicle.update(input.sample(), static_cast<float>(step));
    });

    const auto& state = vehicle.state();
    const float view_z = -5.0f;
    const float ground_z = state.position.z + view_z;

    glClearColor(0.025f, 0.035f, 0.055f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    // A simple oversized floor marker keeps the prototype readable while driving.
    draw_cube(Mat4::translation({state.position.x, -1.1f, ground_z}), 0.16f, 0.18f, 0.21f);

    const Mat4 car = gta4web::math::multiply(
        Mat4::translation({0.0f, 0.0f, view_z}),
        Mat4::rotation_y(state.yaw));
    draw_cube(car, 0.85f, 0.12f, 0.08f);
}

} // namespace

namespace gta4web {

bool App::initialize() {
    program = create_program();
    if (!program) return false;
    mvp_uniform = glGetUniformLocation(program, "u_mvp");
    color_uniform = glGetUniformLocation(program, "u_color");
    glEnable(GL_DEPTH_TEST);
    vehicle.state().position = {0.0f, 0.0f, 0.0f};
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, on_key);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, on_key);
    resize_canvas();
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
