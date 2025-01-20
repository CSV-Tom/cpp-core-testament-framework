class TestStatus {
public:
    enum class Status {
        NotRun,
        Passed,
        Failed,
        Skipped
    };

private:
    Status currentStatus = Status::NotRun;

public:
    // Konstruktor
    TestStatus() = default;
    explicit TestStatus(Status initialStatus) : currentStatus(initialStatus) {}

    // Getter
    Status getStatus() const {
        return currentStatus;
    }

    // Setter
    void setStatus(Status newStatus) {
        currentStatus = newStatus;
    }

    // Zuweisungsoperator
    TestStatus& operator=(Status newStatus) {
        currentStatus = newStatus;
        return *this;
    }

    // Vergleichsoperatoren
    bool operator==(Status otherStatus) const {
        return currentStatus == otherStatus;
    }

    bool operator!=(Status otherStatus) const {
        return currentStatus != otherStatus;
    }

    // Statusprüfungen
    bool isPassed() const {
        return currentStatus == Status::Passed;
    }

    bool isSkipped() const {
        return currentStatus == Status::Skipped;
    }

    bool isFailed() const {
        return currentStatus == Status::Failed;
    }
};