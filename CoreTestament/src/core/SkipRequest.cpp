#include "Testament/Skip.hpp"
#include "Testament/SkipRequest.hpp"

#include <utility>

namespace Testament {

class SkipRequest::Impl {
public:
    explicit Impl(std::string reason_)
        : reason(std::move(reason_)),
          description(reason.empty() ? "test skipped" : "test skipped: " + reason) {}

    std::string reason;
    std::string description;
};

SkipRequest::SkipRequest(std::string reason)
    : impl(std::make_shared<Impl>(std::move(reason))) {}

SkipRequest::~SkipRequest() = default;
SkipRequest::SkipRequest(const SkipRequest&) noexcept = default;
SkipRequest& SkipRequest::operator=(const SkipRequest&) noexcept = default;
SkipRequest::SkipRequest(SkipRequest&& other) noexcept : impl(other.impl) {}
SkipRequest& SkipRequest::operator=(SkipRequest&& other) noexcept {
    impl = other.impl;
    return *this;
}

const char* SkipRequest::what() const noexcept { return impl->description.c_str(); }
std::string_view SkipRequest::reason() const noexcept { return impl->reason; }

[[noreturn]] void skip(std::string_view reason) {
    throw SkipRequest{std::string{reason}};
}

}
