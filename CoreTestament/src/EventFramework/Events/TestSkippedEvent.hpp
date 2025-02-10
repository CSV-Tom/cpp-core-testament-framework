#ifndef TESTFRAMEWORK_TESTAMENT_TESTSKIPPEDEVENT_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTSKIPPEDEVENT_HPP

#include <string>

#include "EventFramework/BaseEvent.hpp"

class TestSkippedEvent : public BaseEvent {
public:
    explicit TestSkippedEvent(const std::string& message) : message(message) {}
    std::string message;
};


#endif