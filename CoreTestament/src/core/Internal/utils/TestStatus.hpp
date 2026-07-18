#ifndef TESTFRAMEWORK_TESTAMENT_TESTSTATUS_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTSTATUS_HPP

namespace Testament {

class TestStatus {
public:
    enum class Status {
        NotRun,
        Passed,
        Failed,
        Skipped
    };

    TestStatus() = default;
    explicit TestStatus(Status initialStatus) : currentStatus(initialStatus) {}

    [[nodiscard]] Status getStatus() const noexcept {
        return currentStatus;
    }

    void setStatus(Status newStatus) noexcept {
        currentStatus = newStatus;
    }

    TestStatus& operator=(Status newStatus) noexcept {
        currentStatus = newStatus;
        return *this;
    }

    [[nodiscard]] bool operator==(Status otherStatus) const noexcept {
        return currentStatus == otherStatus;
    }

    [[nodiscard]] bool operator!=(Status otherStatus) const noexcept {
        return currentStatus != otherStatus;
    }

    [[nodiscard]] bool isPassed() const noexcept {
        return currentStatus == Status::Passed;
    }

    [[nodiscard]] bool isSkipped() const noexcept {
        return currentStatus == Status::Skipped;
    }

    [[nodiscard]] bool isFailed() const noexcept {
        return currentStatus == Status::Failed;
    }

private:
    Status currentStatus{Status::NotRun};
};

}

#endif
