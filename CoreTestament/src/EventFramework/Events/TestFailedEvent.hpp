#ifndef TESTFRAMEWORK_TESTAMENT_TESTFAILEDEVENT_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTFAILEDEVENT_HPP


#include <string>

#include "EventFramework/BaseEvent.hpp"

#include <exception>


class TestFailedEvent : public BaseEvent {
public:
    explicit TestFailedEvent(const std::string& messag, std::exception_ptr exception) : exception(exception) {}
    std::string message;
    std::exception_ptr exception;
};


#endif