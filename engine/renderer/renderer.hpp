#pragma once

#include "camera.hpp"

namespace gta4web::renderer {

class Renderer {
public:
    bool initialize();
    void resize(int width, int height);
    void begin_frame(float time_seconds);
    void draw_test_scene(float time_seconds);
    void end_frame();
    void shutdown();

    Camera& camera() { return camera_; }
    const Camera& camera() const { return camera_; }

private:
    Camera camera_;
    int width_ = 1;
    int height_ = 1;
};

} // namespace gta4web::renderer
