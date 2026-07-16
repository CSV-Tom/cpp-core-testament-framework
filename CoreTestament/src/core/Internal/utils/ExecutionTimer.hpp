#ifndef TESTFRAMEWORK_TESTAMENT_EXECUTIONTIMER_HPP
#define TESTFRAMEWORK_TESTAMENT_EXECUTIONTIMER_HPP

#include <chrono>
#include <stdexcept>

namespace Testament {

class ExecutionTimer {
private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point startTime;
    Clock::time_point endTime;
    std::chrono::duration<double> accumulatedDuration{0};

public:
    void start() {
        startTime = Clock::now();
    }

    void stop() {
        endTime = Clock::now();
        accumulatedDuration += (endTime - startTime);
    }

    std::chrono::duration<double> getDuration() const {
        if (startTime == Clock::time_point{}) {
            throw std::logic_error("Timer was not started before querying elapsed time!");
        }
        if (endTime == Clock::time_point{}) {
            return accumulatedDuration + (Clock::now() - startTime);
        }
        return accumulatedDuration;
    }

    void reset() {
        startTime = Clock::time_point{};
        endTime = Clock::time_point{};
        accumulatedDuration = std::chrono::duration<double>::zero();
    }

    // Operator+, um zwei Timer-Zeiten zu addieren
    ExecutionTimer operator+(const ExecutionTimer& other) const {
        ExecutionTimer result;
        result.accumulatedDuration = this->accumulatedDuration + other.accumulatedDuration;
        return result;
    }

    // Operator+= für einfaches Akkumulieren von Zeiten
    ExecutionTimer& operator+=(const ExecutionTimer& other) {
        this->accumulatedDuration += other.accumulatedDuration;
        return *this;
    }
};

}

#endif
