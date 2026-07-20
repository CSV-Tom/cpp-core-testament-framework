#include "Testament/Trace.hpp"

#include "runtime/TraceContext.hpp"

#include <source_location>
#include <string>
#include <vector>

namespace Testament::detail {

namespace {

struct TraceEntry {
    std::string message;
    std::source_location location;
};

thread_local std::vector<TraceEntry> traces;

}

void pushTrace(std::string_view message, std::source_location location) {
    traces.push_back({std::string{message}, location});
}

void popTrace() noexcept {
    if (!traces.empty()) traces.pop_back();
}

std::string currentTrace() {
    if (traces.empty()) return {};
    std::string result{"\ntrace:"};
    for (auto iterator = traces.rbegin(); iterator != traces.rend(); ++iterator) {
        result += "\n  " + iterator->message + " at " + iterator->location.file_name()
            + ':' + std::to_string(iterator->location.line());
    }
    return result;
}

}

namespace Testament {

Trace::Trace(std::string_view message, std::source_location location)
{
    detail::pushTrace(message, location);
}

Trace::~Trace() {
    detail::popTrace();
}

}
