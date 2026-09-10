#include "gta4web/app.hpp"

#include <cmath>

#include <GLES2/gl2.h>
#include <emscripten.h>
#include <emscripten/html5.h>

namespace {

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
        uniform float u_time;
        varying float v_depth;

        void main() {
            float c = cos(u_time);
            float s = sin(u_time);
            mat3 rotation = mat3(
                c, 0.0, s,
                0.0, 1.0, 0.0,
                -s, 0.0, c
            );
            vec3 p = rotation * a_position;
            p.z -= 4.0;
            float perspective = 1.0 / max(0.5, -p.z);
            gl_Position = vec4(p.x * perspective, p.y * perspective, p.z / 6.0, 1.0);
            v_depth = -p.z;
        }
    )GLSL";

    static constexpr const char* fragment_source = R"GLSL(
        precision mediump float;
        varying float v_depth;

        void main() {
            float light = clamp(1.25 - v_depth * 0.18, 0.25, 1.0);
            gl_FragColor = vec4(0.12 * light, 0.55 * light, 1.0 * light, 1.0);
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
    -1, -1, -1,  1, -1, -1,  1,  1, -1,
    -1, -1, -1,  1,  1, -1, -1,  1, -1,
    -1, -1,  1,  1, -1,  1,  1,  1,  1,
    -1, -1,  1,  1,  1,  1, -1,  1,  1,
    -1, -1, -1, -1,  1, -1, -1,  1,  1,
    -1, -1, -1, -1,  1,  1, -1, -1,  1,
     1, -1, -1,  1,  1, -1,  1,  1,  1,
     1, -1, -1,  1,  1,  1,  1, -1,  1,
    -1, -1, -1,  1, -1, -1,  1, -1,  1,
    -1, -1, -1,  1, -1,  1, -1, -1,  1,
    -1,  1, -1,  1,  1, -1,  1,  1,  1,
    -1,  1, -1,  1,  1,  1, -1,  1,  1
};

GLuint program = 0;
GLint time_uniform = -1;

void resize_canvas() {
    int width = 0;
    int height = 0;
    if (emscripten_get_canvas_element_size("#canvas", &width, &height) == EMSCRIPTEN_RESULT_SUCCESS) {
        glViewport(0, 0, width, height);
    }
}

void render_frame() {
    const double time = emscripten_get_now() * 0.001;
    resize_canvas();

    glClearColor(0.025f, 0.035f, 0.055f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glUniform1f(time_uniform, static_cast<float>(time));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

} // namespace

namespace gta4web {

bool App::initialize() {
    program = create_program();
    if (!program) {
        return false;
    }

    time_uniform = glGetUniformLocation(program, "u_time");
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
