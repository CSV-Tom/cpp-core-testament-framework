module;

#include <memory>

module TestFramework.ExecutionTimer;

import :Implementation; // Importiert die interne Implementierung

ExecutionTimer::ExecutionTimer() : pImpl(std::make_unique<ExecutionTimerImpl>()) {}

ExecutionTimer::~ExecutionTimer() = default;

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
