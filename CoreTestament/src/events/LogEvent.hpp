#include <string>

#include "BaseEvent.hpp"

class LogEvent : public BaseEvent {
public:
    explicit LogEvent(const std::string& message) : message(message) {}
    std::string message;
};
