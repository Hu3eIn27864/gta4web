#pragma once

namespace gta4web {

class App {
public:
    bool initialize();
    void frame(double time_seconds);
    void shutdown();

private:
    int width_ = 1280;
    int height_ = 720;
};

} // namespace gta4web
