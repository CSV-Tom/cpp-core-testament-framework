module;

#include <chrono>

module TestFramework.ExecutionTimer:Implementation;

import TestFramework.ExecutionTimer;


class ExecutionTimer::ExecutionTimerImpl {
public:
    ExecutionTimerImpl() : startTime{}, endTime{}, isRunning{false} {}

    void start() {
        isRunning = true;
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        if (!isRunning) {
            throw std::runtime_error("Timer was not running!");
        }
        endTime = std::chrono::high_resolution_clock::now();
        isRunning = false;
    }

    std::chrono::duration<double> getDuration() const {
        if (isRunning) {
            return std::chrono::high_resolution_clock::now() - startTime;
        }
        return endTime - startTime;
    }

    void reset() {
        isRunning = false;
        startTime = {};
        endTime = {};
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isRunning;
};
