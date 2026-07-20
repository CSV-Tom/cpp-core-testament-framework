#pragma once

#include <chrono>
#include <stdexcept>

namespace Testament::detail {

class Stopwatch {
private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point mStartTime;
    Clock::time_point mEndTime;
    std::chrono::duration<double> mElapsed{0};

public:
    void start() {
        mStartTime = Clock::now();
    }

    void stop() {
        mEndTime = Clock::now();
        mElapsed += (mEndTime - mStartTime);
    }

    [[nodiscard]] std::chrono::duration<double> duration() const {
        if (mStartTime == Clock::time_point{}) {
            throw std::logic_error("Timer was not started before querying elapsed time!");
        }
        if (mEndTime == Clock::time_point{}) {
            return mElapsed + (Clock::now() - mStartTime);
        }
        return mElapsed;
    }

    void reset() {
        mStartTime = Clock::time_point{};
        mEndTime = Clock::time_point{};
        mElapsed = std::chrono::duration<double>::zero();
    }

    [[nodiscard]] Stopwatch operator+(const Stopwatch& other) const noexcept {
        Stopwatch result;
        result.mElapsed = this->mElapsed + other.mElapsed;
        return result;
    }

    Stopwatch& operator+=(const Stopwatch& other) noexcept {
        this->mElapsed += other.mElapsed;
        return *this;
    }
};

}
