#pragma once

#include <algorithm>

namespace gta4web::core {

class FixedTimestep {
public:
    explicit FixedTimestep(double step_seconds = 1.0 / 60.0)
        : step_seconds_(step_seconds) {}

    template <typename UpdateFn>
    void advance(double frame_seconds, UpdateFn&& update) {
        const double clamped = std::min(frame_seconds, max_frame_seconds_);
        accumulator_ += clamped;

        while (accumulator_ >= step_seconds_) {
            update(step_seconds_);
            accumulator_ -= step_seconds_;
        }
    }

    void reset() { accumulator_ = 0.0; }
    double step_seconds() const { return step_seconds_; }

private:
    double step_seconds_;
    double accumulator_ = 0.0;
    double max_frame_seconds_ = 0.25;
};

} // namespace gta4web::core
