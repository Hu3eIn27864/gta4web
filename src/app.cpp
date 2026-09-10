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
    GLint ok = GL_FALSE; glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) { glDeleteShader(shader); return 0; }
    return shader;
}
GLuint create_program() {
    static constexpr const char* vs_src = R"GLSL(attribute vec3 a_position; uniform mat4 u_mvp; void main(){gl_Position=u_mvp*vec4(a_position,1.0);})GLSL";
    static constexpr const char* fs_src = R"GLSL(precision mediump float; uniform vec4 u_color; void main(){gl_FragColor=u_color;})GLSL";
    const GLuint vs=compile_shader(GL_VERTEX_SHADER,vs_src), fs=compile_shader(GL_FRAGMENT_SHADER,fs_src);
    if(!vs||!fs)return 0; const GLuint p=glCreateProgram(); glAttachShader(p,vs);glAttachShader(p,fs);glBindAttribLocation(p,0,"a_position");glLinkProgram(p);glDeleteShader(vs);glDeleteShader(fs);GLint ok=GL_FALSE;glGetProgramiv(p,GL_LINK_STATUS,&ok);if(!ok){glDeleteProgram(p);return 0;}return p;
}
constexpr float cube_vertices[] = {
 -1,-1,-1,1,-1,-1,1,1,-1,-1,-1,1,1,-1,-1,1,-1,-1,-1,1,1,-1,1,1,1,1,1,-1,-1,1,1,1,1,-1,1,1,
 -1,-1,-1,-1,1,-1,-1,1,1,-1,-1,-1,-1,1,1,-1,-1,1,1,-1,-1,1,1,1,1,1,-1,1,1,1,1,-1,1,
 -1,-1,-1,1,-1,-1,1,-1,1,-1,-1,-1,1,-1,1,-1,-1,1,-1,1,-1,1,1,-1,1,1,1,-1,1,-1,1,1,1,-1,1,1
};
GLuint program=0; GLint mvp_uniform=-1,color_uniform=-1; Camera camera; Vehicle vehicle; VehicleInputState input; gta4web::core::FixedTimestep fixed_step;
constexpr Vec3 vehicle_half_extents{1.05f,0.50f,2.00f};
constexpr Aabb collision_blocks[]={{{5.0f,0.0f,7.0f},{8.0f,1.0f,8.0f}},{{-8.0f,0.0f,-3.0f},{-5.0f,1.0f,5.0f}}};
enum KeyCode:int{KeyW=87,KeyA=65,KeyS=83,KeyD=68,KeySpace=32};
EM_BOOL on_key(int type,const EmscriptenKeyboardEvent* e,void*){const bool p=type==EMSCRIPTEN_EVENT_KEYDOWN;if(e->repeat&&p)return EM_TRUE;switch(e->keyCode){case KeyW:input.press_forward(p);break;case KeyA:input.press_left(p);break;case KeyS:input.press_reverse(p);break;case KeyD:input.press_right(p);break;case KeySpace:input.press_handbrake(p);break;default:return EM_FALSE;}return EM_TRUE;}
void resize_canvas(){int w=0,h=0;if(emscripten_get_canvas_element_size("#canvas",&w,&h)==EMSCRIPTEN_RESULT_SUCCESS){glViewport(0,0,w,h);camera.set_aspect(float(w)/float(std::max(h,1)));}}
Mat4 vehicle_rotation(float yaw,float pitch,float roll){return gta4web::math::multiply(Mat4::rotation_y(yaw),gta4web::math::multiply(Mat4::rotation_x(pitch),Mat4::rotation_z(roll)));}
Mat4 model_transform(Vec3 pos,Vec3 scale,float yaw=0,float pitch=0,float roll=0){return gta4web::math::multiply(Mat4::translation(pos),gta4web::math::multiply(vehicle_rotation(yaw,pitch,roll),Mat4::scale(scale)));}
void draw_cube(Vec3 pos,Vec3 scale,float r,float g,float b,float yaw=0,float pitch=0,float roll=0){const Mat4 mvp=gta4web::math::multiply(camera.view_projection(),model_transform(pos,scale,yaw,pitch,roll));glUniformMatrix4fv(mvp_uniform,1,GL_FALSE,mvp.m);glUniform4f(color_uniform,r,g,b,1);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,cube_vertices);glEnableVertexAttribArray(0);glDrawArrays(GL_TRIANGLES,0,36);}
Vec3 local_to_world(Vec3 p,Vec3 l,float yaw,float pitch=0,float roll=0){const float cy=std::cos(yaw),sy=std::sin(yaw);Vec3 r{l.x,l.y*std::cos(roll)-l.z*std::sin(roll),l.y*std::sin(roll)+l.z*std::cos(roll)};Vec3 q{r.x*std::cos(pitch)+r.z*std::sin(pitch),r.y,-r.x*std::sin(pitch)+r.z*std::cos(pitch)};return p+Vec3{q.x*cy+q.z*sy,q.y,-q.x*sy+q.z*cy};}
void resolve_vehicle_collisions(){auto&s=vehicle.state();Aabb box=gta4web::physics::make_aabb(s.position,vehicle_half_extents);const Contact ground=gta4web::physics::ground_contact(box);if(ground.hit){s.position+=ground.normal*ground.penetration;if(s.velocity.y<0)s.velocity.y=0;box=gta4web::physics::make_aabb(s.position,vehicle_half_extents);}for(const Aabb&o:collision_blocks){const Contact c=gta4web::physics::intersect_aabb(box,o);if(!c.hit)continue;s.position+=c.normal*c.penetration;const float vn=gta4web::math::dot(s.velocity,c.normal);if(vn<0)s.velocity-=c.normal*vn;box=gta4web::physics::make_aabb(s.position,vehicle_half_extents);}}
void update_camera(){const auto&s=vehicle.state();const Vec3 f{std::sin(s.yaw),0,std::cos(s.yaw)};const Vec3 target=s.position+Vec3{0,0.7f,0};camera.set_target(target);camera.set_position(target-f*8.0f+Vec3{0,3.5f,0});}
void draw_vehicle(){const auto&s=vehicle.state();const auto&c=vehicle.config();const Vec3 p=s.position;const float y=s.yaw;
 draw_cube(local_to_world(p,{0,0.48f,0},y,s.pitch,s.roll),{1.06f,0.38f,2.02f},0.62f,0.08f,0.055f,y,s.pitch,s.roll);
 draw_cube(local_to_world(p,{0,0.80f,0.82f},y,s.pitch,s.roll),{0.92f,0.25f,0.88f},0.68f,0.10f,0.07f,y,s.pitch,s.roll);
 draw_cube(local_to_world(p,{0,1.08f,-0.20f},y,s.pitch,s.roll),{0.76f,0.36f,0.92f},0.08f,0.12f,0.17f,y,s.pitch,s.roll);
 draw_cube(local_to_world(p,{0,1.43f,-0.20f},y,s.pitch,s.roll),{0.68f,0.06f,0.78f},0.055f,0.065f,0.08f,y,s.pitch,s.roll);
 draw_cube(local_to_world(p,{0,0.34f,2.04f},y,s.pitch,s.roll),{1.0f,0.13f,0.13f},0.06f,0.06f,0.06f,y,s.pitch,s.roll);
 draw_cube(local_to_world(p,{0,0.34f,-2.04f},y,s.pitch,s.roll),{1.0f,0.13f,0.13f},0.06f,0.06f,0.06f,y,s.pitch,s.roll);
 const Vec3 wl[]={{-1.08f,0.28f,1.25f},{1.08f,0.28f,1.25f},{-1.08f,0.28f,-1.25f},{1.08f,0.28f,-1.25f}};
 for(int i=0;i<4;++i){const auto&w=s.wheels[i];Vec3 l=wl[i];l.y-=std::max(0.0f,c.suspension_rest_length-w.suspension_length)*0.45f;const Vec3 wp=local_to_world(p,l,y,s.pitch,s.roll);const float wy=y+w.steer_angle;draw_cube(wp,{0.23f,0.36f,0.43f},0.015f,0.015f,0.018f,wy,s.pitch,s.roll);draw_cube(wp+Vec3{0,0,0.02f},{0.11f,0.22f,0.20f},0.17f,0.18f,0.19f,wy,s.pitch,s.roll);}
 draw_cube(local_to_world(p,{-0.58f,0.56f,1.94f},y,s.pitch,s.roll),{0.20f,0.11f,0.09f},0.95f,0.82f,0.35f,y,s.pitch,s.roll);draw_cube(local_to_world(p,{0.58f,0.56f,1.94f},y,s.pitch,s.roll),{0.20f,0.11f,0.09f},0.95f,0.82f,0.35f,y,s.pitch,s.roll);
}
void render_frame(){static double last=emscripten_get_now()*0.001;const double now=emscripten_get_now()*0.001;const double dt=std::clamp(now-last,0.0,0.25);last=now;resize_canvas();fixed_step.advance(dt,[](double step){vehicle.update(input.sample(),float(step));resolve_vehicle_collisions();});update_camera();glClearColor(0.025f,0.035f,0.055f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glUseProgram(program);draw_cube({0,-0.1f,0},{45,0.1f,45},0.18f,0.20f,0.19f);for(const Aabb&o:collision_blocks){const Vec3 center=(o.min+o.max)*0.5f,half=(o.max-o.min)*0.5f;draw_cube(center,half,0.28f,0.30f,0.33f);}draw_vehicle();}
}
namespace gta4web{bool App::initialize(){program=create_program();if(!program)return false;mvp_uniform=glGetUniformLocation(program,"u_mvp");color_uniform=glGetUniformLocation(program,"u_color");glEnable(GL_DEPTH_TEST);vehicle.state().position={0,0.5f,0};vehicle.state().velocity={};emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,nullptr,EM_TRUE,on_key);emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,nullptr,EM_TRUE,on_key);resize_canvas();update_camera();return true;}void App::frame(double){render_frame();}void App::shutdown(){if(program){glDeleteProgram(program);program=0;}}}
int main(){gta4web::App app;if(!app.initialize())return 1;emscripten_set_main_loop([](){render_frame();},0,EM_TRUE);return 0;}
