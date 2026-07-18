#include "Testament/Runner.hpp"
#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"
#include "core/Internal/InternalSuite.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

int main() {
    const auto reportPath = std::filesystem::current_path() / "junit-reporter-validation.xml";
    std::error_code removeError;
    std::filesystem::remove(reportPath, removeError);

    auto suite = Testament::InternalRegistry::getInstance().registerSuite(
        std::make_shared<Testament::InternalSuite>("suite <&>")
    );
    suite->addTest(Testament::detail::RuntimeBridge::makeTest("passing test", {}, [] {}));
    suite->addTest(Testament::detail::RuntimeBridge::makeTest("failing \"test\"", {}, [] {
        throw std::runtime_error("failure <reason> & details");
    }));

    auto lifecycleSuite = Testament::InternalRegistry::getInstance().registerSuite(
        std::make_shared<Testament::InternalSuite>("lifecycle suite")
    );
    lifecycleSuite->setBeforeEach([] {
        throw std::runtime_error("setup <failed>");
    });
    lifecycleSuite->addTest(Testament::detail::RuntimeBridge::makeTest("lifecycle test", {}, [] {}));

    auto parameterSuite = Testament::Suite(
        "parameter suite",
        Testament::ParameterizedTest(
            "values",
            Testament::Cases(
                Testament::TestCase("one", 1),
                Testament::TestCase("two", 2)
            ),
            [](int) {}
        )
    );

    std::string executable = "JUnitReporterValidation";
    std::string missingPathOption = "--junit";
    char* invalidArguments[]{executable.data(), missingPathOption.data()};
    const int invalidArgumentsExitCode = Testament::run(2, invalidArguments);

    std::string option = "--junit=" + reportPath.string();
    char* arguments[]{executable.data(), option.data()};
    const int exitCode = Testament::run(2, arguments);

    std::ifstream report(reportPath);
    const std::string xml{std::istreambuf_iterator<char>{report}, {}};
    std::filesystem::remove(reportPath, removeError);

    return invalidArgumentsExitCode == 2
        && exitCode == 1
        && parameterSuite
        && xml.contains("<testsuites tests=\"5\" failures=\"1\" errors=\"1\" skipped=\"0\"")
        && xml.contains("<testsuite name=\"suite &lt;&amp;&gt;\"")
        && xml.contains("name=\"failing &quot;test&quot;\"")
        && xml.contains("failure &lt;reason&gt; &amp; details")
        && xml.contains("<error message=\"Error in beforeEach: setup &lt;failed&gt;\"")
        && xml.contains("name=\"values / one\"")
        && xml.contains("name=\"values / two\"")
        ? 0
        : 1;
}
