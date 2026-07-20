#pragma once

namespace Testament::detail {

class TestState {
public:
    enum class Status {
        NotRun,
        Passed,
        Failed,
        Skipped
    };

    TestState() = default;
    explicit TestState(Status initialStatus) : mCurrentStatus(initialStatus) {}

    [[nodiscard]] Status status() const noexcept {
        return mCurrentStatus;
    }

    void setStatus(Status newStatus) noexcept {
        mCurrentStatus = newStatus;
    }

    TestState& operator=(Status newStatus) noexcept {
        mCurrentStatus = newStatus;
        return *this;
    }

    [[nodiscard]] bool operator==(Status otherStatus) const noexcept {
        return mCurrentStatus == otherStatus;
    }

    [[nodiscard]] bool operator!=(Status otherStatus) const noexcept {
        return mCurrentStatus != otherStatus;
    }

    [[nodiscard]] bool isPassed() const noexcept {
        return mCurrentStatus == Status::Passed;
    }

    [[nodiscard]] bool isSkipped() const noexcept {
        return mCurrentStatus == Status::Skipped;
    }

    [[nodiscard]] bool isFailed() const noexcept {
        return mCurrentStatus == Status::Failed;
    }

private:
    Status mCurrentStatus{Status::NotRun};
};

}
