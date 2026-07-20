#include "Testament/detail/RuntimeBridge.hpp"

#include "Testament/TestOptions.hpp"

#include "Internal/FunctionVariant.hpp"
#include "Internal/InternalTest.hpp"
#include "Internal/TestAccess.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace Testament {

class detail::TestHandle::Impl {
public:
    explicit Impl(std::unique_ptr<InternalTest> internalTest)
        : test(std::move(internalTest)) {}

    std::unique_ptr<InternalTest> test;
};

detail::TestHandle detail::RuntimeBridge::makeTest(
    std::string_view name, TestOptions options, std::move_only_function<void()> function,
    std::source_location location
) {
    return TestHandle{std::make_unique<TestHandle::Impl>(
        std::make_unique<InternalTest>(std::string{name}, location, std::move(options),
                                       FunctionVariant{std::move(function)})
    )};
}

detail::TestHandle detail::RuntimeBridge::makeTest(
    std::string_view name, TestOptions options, std::type_index fixtureType,
    std::move_only_function<void(LifecycleSuite&)> function, std::source_location location
) {
    return TestHandle{std::make_unique<TestHandle::Impl>(
        std::make_unique<InternalTest>(std::string{name}, location, std::move(options),
                                       FunctionVariant{std::move(function)}, fixtureType)
    )};
}

std::unique_ptr<InternalTest> detail::TestAccess::release(TestHandle&& test) {
    if (!test.pImpl || !test.pImpl->test) {
        throw std::invalid_argument("Cannot add an empty or moved-from test");
    }
    return std::move(test.pImpl->test);
}

detail::TestHandle::TestHandle(std::unique_ptr<Impl> implementation)
    : pImpl(std::move(implementation)) {}
detail::TestHandle::~TestHandle() = default;
detail::TestHandle::TestHandle(TestHandle&&) noexcept = default;
detail::TestHandle& detail::TestHandle::operator=(TestHandle&&) noexcept = default;

}
