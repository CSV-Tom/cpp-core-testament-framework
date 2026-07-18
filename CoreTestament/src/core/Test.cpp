#include "Testament/SuiteRegistration.hpp"

#include "Testament/Options.hpp"

#include "Internal/FunctionVariant.hpp"
#include "Internal/InternalTest.hpp"
#include "Internal/TestAccess.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace Testament {

class detail::TestHandle::Impl {
public:
    explicit Impl(std::unique_ptr<InternalTest> test_) : test(std::move(test_)) {}

    std::unique_ptr<InternalTest> test;
};

detail::TestHandle detail::makeRuntimeTest(std::string_view name, TestOptions options,
                                          std::function<void()> function) {
    return TestHandle{std::make_unique<TestHandle::Impl>(
        std::make_unique<InternalTest>(std::string{name}, std::move(options),
                                       FunctionVariant{std::move(function)})
    )};
}

detail::TestHandle detail::makeRuntimeTest(std::string_view name, TestOptions options,
                                          std::type_index fixtureType,
                                          std::function<void(LifecycleSuite&)> function) {
    return TestHandle{std::make_unique<TestHandle::Impl>(
        std::make_unique<InternalTest>(std::string{name}, std::move(options),
                                       FunctionVariant{std::move(function)}, fixtureType)
    )};
}

std::unique_ptr<InternalTest> detail::TestAccess::release(TestHandle&& test) {
    if (!test.impl || !test.impl->test) {
        throw std::invalid_argument("Cannot add an empty or moved-from test");
    }
    return std::move(test.impl->test);
}

detail::TestHandle::TestHandle(std::unique_ptr<Impl> impl_) : impl(std::move(impl_)) {}
detail::TestHandle::~TestHandle() = default;
detail::TestHandle::TestHandle(TestHandle&&) noexcept = default;
detail::TestHandle& detail::TestHandle::operator=(TestHandle&&) noexcept = default;

}
