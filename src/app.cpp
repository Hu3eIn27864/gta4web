#include "gta4web/app.hpp"
#include "camera.hpp"
#include "math.hpp"

#include <cmath>

#include <GLES2/gl2.h>
#include <emscripten.h>
#include <emscripten/html5.h>

namespace {

using gta4web::math::Mat4;
using gta4web::renderer::Camera;

GLuint compile_shader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint create_program() {
    static constexpr const char* vertex_source = R"GLSL(
        attribute vec3 a_position;
        uniform mat4 u_mvp;
        varying float v_depth;

        void main() {
            vec4 world = vec4(a_position, 1.0);
            gl_Position = u_mvp * world;
            v_depth = world.z;
        }
    )GLSL";

    static constexpr const char* fragment_source = R"GLSL(
        precision mediump float;
        varying float v_depth;

        void main() {
            float light = clamp(1.0 - abs(v_depth) * 0.12, 0.3, 1.0);
            gl_FragColor = vec4(0.10 * light, 0.50 * light, 1.0 * light, 1.0);
        }
    )GLSL";

    const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (!vertex_shader || !fragment_shader) {
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glBindAttribLocation(program, 0, "a_position");
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

constexpr float cube_vertices[] = {
    -1,-1,-1,  1,-1,-1,  1, 1,-1,  -1,-1,-1,  1,1,-1,  -1,1,-1,
    -1,-1, 1,  1,-1, 1,  1, 1, 1,  -1,-1, 1,  1,1,1,  -1,1,1,
    -1,-1,-1, -1,1,-1, -1,1,1,  -1,-1,-1, -1,1,1,  -1,-1,1,
     1,-1,-1,  1,1,-1,  1,1,1,   1,-1,-1,  1,1,1,   1,-1,1,
    -1,-1,-1,  1,-1,-1,  1,-1,1, -1,-1,-1,  1,-1,1, -1,-1,1,
    -1,1,-1,   1,1,-1,   1,1,1,  -1,1,-1,   1,1,1,  -1,1,1
};

GLuint program = 0;
GLint mvp_uniform = -1;
Camera camera;

void resize_canvas() {
    int width = 0;
    int height = 0;
    if (emscripten_get_canvas_element_size("#canvas", &width, &height) == EMSCRIPTEN_RESULT_SUCCESS) {
        glViewport(0, 0, width, height);
        camera.set_aspect(static_cast<float>(width) / static_cast<float>(height > 0 ? height : 1));
    }
}

void render_frame() {
    const float time = static_cast<float>(emscripten_get_now() * 0.001);
    resize_canvas();

    const Mat4 model = gta4web::math::multiply(
        Mat4::translation({0.0f, 0.0f, -5.0f}),
        Mat4::rotation_y(time));
    const Mat4 mvp = gta4web::math::multiply(camera.projection(), model);

    glClearColor(0.025f, 0.035f, 0.055f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, mvp.m);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, cube_vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

} // namespace

namespace gta4web {

bool App::initialize() {
    program = create_program();
    if (!program) {
        return false;
    }

    mvp_uniform = glGetUniformLocation(program, "u_mvp");
    glEnable(GL_DEPTH_TEST);
    resize_canvas();
    return true;
}

void App::frame(double) {
    render_frame();
}

void App::shutdown() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}

} // namespace gta4web

int main() {
    gta4web::App app;
    if (!app.initialize()) {
        return 1;
    }

    emscripten_set_main_loop([]() { render_frame(); }, 0, EM_TRUE);
    return 0;
}
