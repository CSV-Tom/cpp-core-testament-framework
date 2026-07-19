#ifndef TESTAMENT_INTERNAL_SUITEASSEMBLER_HPP
#define TESTAMENT_INTERNAL_SUITEASSEMBLER_HPP

#include <memory>
#include <functional>
#include <string>
#include <typeindex>
#include <vector>

namespace Testament {

class InternalSuite;
class LifecycleSuite;
class SuiteOptions;

namespace detail {
class TestHandle;
}

class SuiteAssembler {
public:
    [[nodiscard]] static std::shared_ptr<InternalSuite> assemble(
        std::string name, std::type_index fixtureType,
        std::move_only_function<std::unique_ptr<LifecycleSuite>()> fixtureFactory,
        SuiteOptions options, std::vector<detail::TestHandle> tests
    );
};

}

#endif
