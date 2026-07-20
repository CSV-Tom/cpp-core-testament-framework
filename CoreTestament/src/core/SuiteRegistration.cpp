#include "Testament/detail/RuntimeBridge.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/SuiteOptions.hpp"

#include "Internal/InternalRegistry.hpp"
#include "Internal/SuiteAssembler.hpp"

#include <string>
#include <optional>
#include <utility>

namespace Testament {

class SuiteRegistration::Impl {
public:
    explicit Impl(std::shared_ptr<InternalSuite> internalSuite)
        : suite(std::move(internalSuite)) {}
    explicit Impl(ConfigurationDiagnostics::Id configurationDiagnostic)
        : diagnosticId(configurationDiagnostic) {}

    ~Impl() {
        if (suite) InternalRegistry::instance().unregisterSuite(suite);
        if (diagnosticId) {
            InternalRegistry::instance().removeConfigurationError(*diagnosticId);
        }
    }

    std::shared_ptr<InternalSuite> suite;
    std::optional<ConfigurationDiagnostics::Id> diagnosticId;
};

SuiteRegistration detail::RuntimeBridge::registerSuite(std::string_view name,
                                                       std::source_location location,
                                                       SuiteOptions options,
                                                       std::vector<TestHandle> tests) {
    try {
        auto suite = SuiteAssembler::assemble(
            std::string{name}, location, std::type_index(typeid(void)), {},
            std::move(options), std::move(tests)
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
        auto suite = SuiteAssembler::assemble(
            std::string{name}, location, fixtureType, std::move(fixtureFactory),
            std::move(options), std::move(tests)
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
    const auto id = InternalRegistry::instance().recordConfigurationError(std::move(error));
    return SuiteRegistration{std::make_unique<SuiteRegistration::Impl>(id)};
}

SuiteRegistration::SuiteRegistration(std::unique_ptr<Impl> implementation)
    : impl(std::move(implementation)) {}
SuiteRegistration::~SuiteRegistration() = default;
SuiteRegistration::SuiteRegistration(SuiteRegistration&&) noexcept = default;
SuiteRegistration& SuiteRegistration::operator=(SuiteRegistration&&) noexcept = default;

SuiteRegistration::operator bool() const noexcept {
    return impl && static_cast<bool>(impl->suite);
}

}
