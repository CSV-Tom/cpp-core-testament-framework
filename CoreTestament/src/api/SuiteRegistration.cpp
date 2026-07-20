#include "Testament/detail/RuntimeBridge.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/SuiteOptions.hpp"

#include "runtime/SuiteRegistry.hpp"
#include "runtime/SuiteFactory.hpp"

#include <string>
#include <optional>
#include <utility>

namespace Testament {

class SuiteRegistration::Impl {
public:
    explicit Impl(std::shared_ptr<detail::SuiteInstance> suiteInstance)
        : suite(std::move(suiteInstance)) {}
    explicit Impl(detail::ConfigurationErrorStore::Id configurationDiagnostic)
        : diagnosticId(configurationDiagnostic) {}

    ~Impl() {
        if (suite) detail::SuiteRegistry::instance().unregisterSuite(suite);
        if (diagnosticId) {
            detail::SuiteRegistry::instance().removeConfigurationError(*diagnosticId);
        }
    }

    std::shared_ptr<detail::SuiteInstance> suite;
    std::optional<detail::ConfigurationErrorStore::Id> diagnosticId;
};

SuiteRegistration detail::RuntimeBridge::registerSuite(std::string_view name,
                                                       std::source_location location,
                                                       SuiteOptions options,
                                                       std::vector<TestHandle> tests) {
    try {
        auto suite = SuiteRegistry::instance().registerSuite(
            SuiteFactory::create(
                std::string{name}, location, std::type_index(typeid(void)), {},
                std::move(options), std::move(tests)
            )
        );
        return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(std::move(suite))};
    } catch (const std::logic_error& error) {
        return RuntimeBridge::configurationError(
            std::string{name} + " at " + location.file_name() + ':'
            + std::to_string(location.line()) + ": " + error.what()
        );
    }
}

SuiteRegistration detail::RuntimeBridge::registerSuite(
    std::string_view name, std::source_location location, std::type_index fixtureType,
    std::move_only_function<std::unique_ptr<LifecycleSuite>()> fixtureFactory,
    SuiteOptions options, std::vector<TestHandle> tests
) {
    try {
        auto suite = SuiteRegistry::instance().registerSuite(
            SuiteFactory::create(
                std::string{name}, location, fixtureType, std::move(fixtureFactory),
                std::move(options), std::move(tests)
            )
        );
        return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(std::move(suite))};
    } catch (const std::logic_error& error) {
        return RuntimeBridge::configurationError(
            std::string{name} + " at " + location.file_name() + ':'
            + std::to_string(location.line()) + ": " + error.what()
        );
    }
}

SuiteRegistration detail::RuntimeBridge::configurationError(std::string error) {
    const auto id = SuiteRegistry::instance().recordConfigurationError(std::move(error));
    return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(id)};
}

SuiteRegistration::SuiteRegistration(std::unique_ptr<Impl> implementation)
    : pImpl(std::move(implementation)) {}
SuiteRegistration::~SuiteRegistration() = default;
SuiteRegistration::SuiteRegistration(SuiteRegistration&&) noexcept = default;
SuiteRegistration& SuiteRegistration::operator=(SuiteRegistration&&) noexcept = default;

SuiteRegistration::operator bool() const noexcept {
    return pImpl && static_cast<bool>(pImpl->suite);
}

}
