#include "Testament/detail/RuntimeBridge.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/SuiteOptions.hpp"

#include "Internal/InternalRegistry.hpp"
#include "Internal/SuiteAssembler.hpp"

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

SuiteRegistration detail::RuntimeBridge::registerSuite(std::string_view name,
                                                       SuiteOptions options,
                                                       std::vector<TestHandle> tests) {
    try {
        auto suite = SuiteAssembler::assemble(
            std::string{name}, nullptr, std::move(options), std::move(tests)
        );
        return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(std::move(suite))};
    } catch (const std::logic_error& error) {
        RuntimeBridge::recordConfigurationError(std::string{name} + ": " + error.what());
        return RuntimeBridge::invalidRegistration();
    }
}

SuiteRegistration detail::RuntimeBridge::registerSuite(
    std::string_view name, std::unique_ptr<LifecycleSuite> fixture,
    SuiteOptions options, std::vector<TestHandle> tests
) {
    try {
        auto suite = SuiteAssembler::assemble(
            std::string{name}, std::move(fixture), std::move(options), std::move(tests)
        );
        return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(std::move(suite))};
    } catch (const std::logic_error& error) {
        RuntimeBridge::recordConfigurationError(std::string{name} + ": " + error.what());
        return RuntimeBridge::invalidRegistration();
    }
}

SuiteRegistration detail::RuntimeBridge::invalidRegistration() {
    return SuiteRegistration{nullptr};
}

void detail::RuntimeBridge::recordConfigurationError(std::string error) {
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
