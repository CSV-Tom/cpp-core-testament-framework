#include "Testament/SuiteRegistration.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/Options.hpp"

#include "Internal/InternalRegistry.hpp"
#include "Internal/InternalSuite.hpp"

#include <string>
#include <utility>

namespace Testament {

class SuiteRegistration::Impl {
public:
    explicit Impl(std::shared_ptr<InternalSuite> suite_) : suite(std::move(suite_)) {}

    ~Impl() {
        InternalRegistry::getInstance().unregisterSuite(suite);
    }

    std::shared_ptr<InternalSuite> suite;
};

namespace {

std::shared_ptr<InternalSuite> assembleSuite(std::string name,
                                            std::unique_ptr<LifecycleSuite> fixture,
                                            SuiteOptions options,
                                            std::vector<detail::TestHandle> tests) {
    auto suite = fixture
        ? std::make_shared<InternalSuite>(std::move(name), std::move(fixture), std::move(options))
        : std::make_shared<InternalSuite>(std::move(name), std::move(options));
    for (auto& test : tests) {
        suite->addTest(std::move(test));
    }
    return InternalRegistry::getInstance().registerSuite(std::move(suite));
}

}

SuiteRegistration detail::registerSuite(std::string_view name, SuiteOptions options,
                                        std::vector<TestHandle> tests) {
    try {
        auto suite = assembleSuite(std::string{name}, nullptr, std::move(options), std::move(tests));
        return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(std::move(suite))};
    } catch (const std::logic_error& error) {
        recordConfigurationError(std::string{name} + ": " + error.what());
        return invalidSuiteRegistration();
    }
}

SuiteRegistration detail::registerSuite(std::string_view name,
                                        std::unique_ptr<LifecycleSuite> fixture,
                                        SuiteOptions options, std::vector<TestHandle> tests) {
    try {
        auto suite = assembleSuite(std::string{name}, std::move(fixture), std::move(options),
                                   std::move(tests));
        return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(std::move(suite))};
    } catch (const std::logic_error& error) {
        recordConfigurationError(std::string{name} + ": " + error.what());
        return invalidSuiteRegistration();
    }
}

SuiteRegistration detail::invalidSuiteRegistration() {
    return SuiteRegistration{nullptr};
}

void detail::recordConfigurationError(std::string error) {
    InternalRegistry::getInstance().recordConfigurationError(std::move(error));
}

SuiteRegistration::SuiteRegistration(std::unique_ptr<Impl> impl_) : impl(std::move(impl_)) {}
SuiteRegistration::~SuiteRegistration() = default;
SuiteRegistration::SuiteRegistration(SuiteRegistration&&) noexcept = default;
SuiteRegistration& SuiteRegistration::operator=(SuiteRegistration&&) noexcept = default;

SuiteRegistration::operator bool() const noexcept {
    return static_cast<bool>(impl);
}

}
