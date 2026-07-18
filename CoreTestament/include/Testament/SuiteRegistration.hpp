#ifndef TESTAMENT_SUITEREGISTRATION_HPP
#define TESTAMENT_SUITEREGISTRATION_HPP

#include <memory>

namespace Testament {

namespace detail {
class RuntimeBridge;
}

class [[nodiscard("the suite registration must remain alive")]] SuiteRegistration {
public:
    ~SuiteRegistration();

    SuiteRegistration(SuiteRegistration&&) noexcept;
    SuiteRegistration& operator=(SuiteRegistration&&) noexcept;

    SuiteRegistration(const SuiteRegistration&) = delete;
    SuiteRegistration& operator=(const SuiteRegistration&) = delete;

    [[nodiscard]] explicit operator bool() const noexcept;

private:
    class Impl;
    explicit SuiteRegistration(std::unique_ptr<Impl> impl);

    friend class detail::RuntimeBridge;

    std::unique_ptr<Impl> impl;
};

}

#endif
