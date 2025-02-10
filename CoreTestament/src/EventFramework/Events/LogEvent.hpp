#ifndef EVENTFRAMEWORK_EVENTS_LOGEVENT_HPP
#define EVENTFRAMEWORK_EVENTS_LOGEVENT_HPP

#include <string>

#include "EventFramework/BaseEvent.hpp"

class LogEvent : public BaseEvent {
public:
    explicit LogEvent(const std::string& message) : message(message) {}
    std::string message;
};


#endif