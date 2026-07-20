#ifndef TESTAMENT_SUITEREGISTRATION_HPP
#define TESTAMENT_SUITEREGISTRATION_HPP

#include "Testament/Export.hpp"

#include <memory>

namespace Testament {

namespace detail {
class RuntimeBridge;
}

class [[nodiscard("the suite registration must remain alive")]] TESTAMENT_EXPORT SuiteRegistration {
public:
    ~SuiteRegistration();

    SuiteRegistration(SuiteRegistration&&) noexcept;
    SuiteRegistration& operator=(SuiteRegistration&&) noexcept;

    SuiteRegistration(const SuiteRegistration&) = delete;
    SuiteRegistration& operator=(const SuiteRegistration&) = delete;

    [[nodiscard]] explicit operator bool() const noexcept;

private:
    class TESTAMENT_NO_EXPORT Impl;
    TESTAMENT_NO_EXPORT explicit SuiteRegistration(std::unique_ptr<Impl> implementation);

    friend class detail::RuntimeBridge;

    std::unique_ptr<Impl> pImpl;
};

}

#endif
