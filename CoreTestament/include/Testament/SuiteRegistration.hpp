#ifndef TESTAMENT_SUITEREGISTRATION_HPP
#define TESTAMENT_SUITEREGISTRATION_HPP

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

namespace Testament {

class LifecycleSuite;
class SuiteOptions;
class TestOptions;
class SuiteRegistration;

namespace detail {

class TestAccess;

class TestHandle {
public:
    ~TestHandle();

    TestHandle(TestHandle&&) noexcept;
    TestHandle& operator=(TestHandle&&) noexcept;

    TestHandle(const TestHandle&) = delete;
    TestHandle& operator=(const TestHandle&) = delete;

private:
    class Impl;
    explicit TestHandle(std::unique_ptr<Impl> impl);

    friend TestHandle makeRuntimeTest(std::string_view, TestOptions, std::function<void()>);
    friend TestHandle makeRuntimeTest(std::string_view, TestOptions, std::type_index,
                                      std::function<void(LifecycleSuite&)>);
    friend class TestAccess;

    std::unique_ptr<Impl> impl;
};

TestHandle makeRuntimeTest(std::string_view name, TestOptions options,
                           std::function<void()> function);
TestHandle makeRuntimeTest(std::string_view name, TestOptions options,
                           std::type_index fixtureType,
                           std::function<void(LifecycleSuite&)> function);

SuiteRegistration registerSuite(std::string_view name, SuiteOptions options,
                                std::vector<TestHandle> tests);
SuiteRegistration registerSuite(std::string_view name,
                                std::unique_ptr<LifecycleSuite> fixture,
                                SuiteOptions options, std::vector<TestHandle> tests);
SuiteRegistration invalidSuiteRegistration();
void recordConfigurationError(std::string message);

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

    friend SuiteRegistration detail::registerSuite(std::string_view, SuiteOptions,
                                                    std::vector<detail::TestHandle>);
    friend SuiteRegistration detail::registerSuite(
        std::string_view, std::unique_ptr<LifecycleSuite>, SuiteOptions,
        std::vector<detail::TestHandle>);
    friend SuiteRegistration detail::invalidSuiteRegistration();

    std::unique_ptr<Impl> impl;
};

}

#endif
