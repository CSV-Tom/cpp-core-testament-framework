#ifndef TESTFRAMEWORK_TESTAMENT_TESTPASSEDEVENT_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTPASSEDEVENT_HPP

#include <string>

#include "EventFramework/BaseEvent.hpp"

class TestPassedEvent : public BaseEvent {
public:
    explicit TestPassedEvent(const std::string& message) : message(message) {}
    std::string message;
};


#endif