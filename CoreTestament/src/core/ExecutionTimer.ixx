
module;

#include <memory>
#include <chrono>

export module TestFramework.ExecutionTimer;

//import std;// Not supported now

export class ExecutionTimer {
public:
    ExecutionTimer();
    ~ExecutionTimer();

    void start();
    void stop();
    std::chrono::duration<double> getDuration() const;
    void reset();
private:
    class ExecutionTimerImpl;
    std::unique_ptr<ExecutionTimerImpl> pImpl;
};

// objdump -t ./CoreTestament/CoreTestament/CMakeFiles/CoreTestamentFramework.dir/src/core/ExecutionTimer.ixx.o