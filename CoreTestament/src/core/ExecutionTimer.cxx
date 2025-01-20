
module;

#include <chrono>
#include <memory>
#include <stdexcept>

module TestFramework.ExecutionTimer;

// Importiere die erforderlichen Header für die Implementierung

// Definition der Implementierungsklasse
class ExecutionTimer::ExecutionTimerImpl {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;

public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
    }

    std::chrono::duration<double> getDuration() const {
        if (startTime == std::chrono::high_resolution_clock::time_point{}) {
            throw std::logic_error("Timer was not started before querying elapsed time!");
        }
        if (endTime == std::chrono::high_resolution_clock::time_point{}) {
            return std::chrono::high_resolution_clock::now() - startTime;
        }
        return endTime - startTime;
    }

    void reset() {
        startTime = std::chrono::high_resolution_clock::time_point{};
        endTime = std::chrono::high_resolution_clock::time_point{};
    }
};

// Konstruktor
ExecutionTimer::ExecutionTimer() : pImpl(std::make_unique<ExecutionTimerImpl>()) {}

// Destruktor
ExecutionTimer::~ExecutionTimer() = default;

// Methoden
void ExecutionTimer::start() {
    pImpl->start();
}

void ExecutionTimer::stop() {
    pImpl->stop();
}

std::chrono::duration<double> ExecutionTimer::getDuration() const {
    return pImpl->getDuration();
}

void ExecutionTimer::reset() {
    pImpl->reset();
}
