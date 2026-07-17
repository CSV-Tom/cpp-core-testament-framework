#include "Testament/Suite.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/Options.hpp"
#include "Testament/Test.hpp"

#include "Internal/InternalRegistry.hpp"
#include "Internal/InternalSuite.hpp"

#include <string>
#include <utility>

namespace Testament {

class Suite::Impl {
public:
    explicit Impl(std::shared_ptr<InternalSuite> suite_) : suite(std::move(suite_)) {}

    ~Impl() {
        InternalRegistry::getInstance().unregisterSuite(suite);
    }

    std::shared_ptr<InternalSuite> suite;
};

namespace {

std::shared_ptr<InternalSuite> registerSuite(std::string name,
                                            std::unique_ptr<LifecycleSuite> fixture,
                                            SuiteOptions options,
                                            std::vector<Test> tests) {
    auto suite = fixture
        ? std::make_shared<InternalSuite>(std::move(name), std::move(fixture), std::move(options))
        : std::make_shared<InternalSuite>(std::move(name), std::move(options));
    for (auto& test : tests) {
        suite->addTest(std::move(test));
    }
    return InternalRegistry::getInstance().registerSuite(std::move(suite));
}

}

Suite detail::makeSuite(std::string_view name, SuiteOptions options, std::vector<Test> tests) {
    auto suite = registerSuite(std::string{name}, nullptr, std::move(options), std::move(tests));
    return Suite{std::make_unique<Suite::Impl>(std::move(suite))};
}

Suite detail::makeSuite(std::string_view name, std::unique_ptr<LifecycleSuite> fixture,
                        SuiteOptions options, std::vector<Test> tests) {
    auto suite = registerSuite(std::string{name}, std::move(fixture), std::move(options),
                               std::move(tests));
    return Suite{std::make_unique<Suite::Impl>(std::move(suite))};
}

Suite::Suite(std::unique_ptr<Impl> impl_) : impl(std::move(impl_)) {}
Suite::~Suite() = default;
Suite::Suite(Suite&&) noexcept = default;
Suite& Suite::operator=(Suite&&) noexcept = default;

Suite::operator bool() const noexcept {
    return static_cast<bool>(impl);
}

}
