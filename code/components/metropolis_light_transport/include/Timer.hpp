#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <chrono>

namespace Metropolis {
    struct Timer {
        std::chrono::time_point<std::chrono::steady_clock> begin{}, end{};
        std::chrono::duration<float> duration{};
        Timer() {}
        void init() {
            duration = std::chrono::duration<float>(0.0);
        }
        void stop() {
            end = std::chrono::high_resolution_clock::now();
            duration += end - begin;
        }

        void start() {
            begin = std::chrono::high_resolution_clock::now();
        }

        float getTime() {
            float ms = duration.count() * 1000.0f;
            return ms;
        }
    };

}

#endif