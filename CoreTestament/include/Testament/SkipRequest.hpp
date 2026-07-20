#ifndef TESTAMENT_SKIPREQUEST_HPP
#define TESTAMENT_SKIPREQUEST_HPP

#include "Testament/Export.hpp"

#include <exception>
#include <memory>
#include <string>
#include <string_view>

namespace Testament {

class TESTAMENT_EXPORT SkipRequest final : public std::exception {
public:
    explicit SkipRequest(std::string reason);
    ~SkipRequest() override;

    SkipRequest(const SkipRequest&) noexcept;
    SkipRequest& operator=(const SkipRequest&) noexcept;
    SkipRequest(SkipRequest&&) noexcept;
    SkipRequest& operator=(SkipRequest&&) noexcept;

    [[nodiscard]] const char* what() const noexcept override;
    [[nodiscard]] std::string_view reason() const noexcept;

private:
    class TESTAMENT_NO_EXPORT Impl;
    std::shared_ptr<const Impl> pImpl;
};

}

#endif
