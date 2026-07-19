#ifndef TESTAMENT_TRACE_HPP
#define TESTAMENT_TRACE_HPP

#include <source_location>
#include <string_view>

namespace Testament {

class Trace final {
public:
    explicit Trace(std::string_view message,
                   std::source_location location = std::source_location::current());
    ~Trace();

    Trace(const Trace&) = delete;
    Trace& operator=(const Trace&) = delete;
    Trace(Trace&&) = delete;
    Trace& operator=(Trace&&) = delete;

};

}

#endif
