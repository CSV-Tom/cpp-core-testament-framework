#include "Testament/Test.hpp"

#include "Testament/Options.hpp"

#include "Internal/FunctionVariant.hpp"
#include "Internal/InternalTest.hpp"
#include "Internal/TestAccess.hpp"

#include <string>
#include <utility>

namespace Testament {

class Test::Impl {
public:
    explicit Impl(std::unique_ptr<InternalTest> test_) : test(std::move(test_)) {}

    std::unique_ptr<InternalTest> test;
};

Test detail::makeTest(std::string_view name, TestOptions options, std::function<void()> function) {
    return Test{std::make_unique<Test::Impl>(
        std::make_unique<InternalTest>(std::string{name}, std::move(options),
                                       FunctionVariant{std::move(function)})
    )};
}

Test detail::makeTest(std::string_view name, TestOptions options,
                      std::function<void(LifecycleSuite&)> function) {
    return Test{std::make_unique<Test::Impl>(
        std::make_unique<InternalTest>(std::string{name}, std::move(options),
                                       FunctionVariant{std::move(function)})
    )};
}

std::unique_ptr<InternalTest> detail::TestAccess::release(Test&& test) {
    return std::move(test.impl->test);
}

Test::Test(std::unique_ptr<Impl> impl_) : impl(std::move(impl_)) {}
Test::~Test() = default;
Test::Test(Test&&) noexcept = default;
Test& Test::operator=(Test&&) noexcept = default;

}
