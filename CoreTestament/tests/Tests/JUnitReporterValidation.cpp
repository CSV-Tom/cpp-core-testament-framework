#include "Testament/Runner.hpp"
#include "Testament/Testament.hpp"

#include "runtime/SuiteRegistry.hpp"
#include "runtime/SuiteInstance.hpp"

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

    auto suiteName = std::string{"suite <&>"} + '\x01';
    auto suite = Testament::detail::SuiteRegistry::instance().registerSuite(
        std::make_shared<Testament::detail::SuiteInstance>(suiteName)
    );
    const auto passingTestLine = __LINE__ + 1;
    suite->addTest(Testament::detail::RuntimeBridge::makeTest("passing test", {}, [] {}));
    suite->addTest(Testament::detail::RuntimeBridge::makeTest("failing \"test\"", {}, [] {
        throw std::runtime_error(std::string{"failure <reason> & details"} + '\x01' + "\xc3\x28");
    }));

    auto lifecycleSuite = Testament::detail::SuiteRegistry::instance().registerSuite(
        std::make_shared<Testament::detail::SuiteInstance>("lifecycle suite")
    );
    lifecycleSuite->setBeforeEach([] {
        throw std::runtime_error("setup <failed>");
    });
    lifecycleSuite->addTest(Testament::detail::RuntimeBridge::makeTest("lifecycle test", {}, [] {}));

    auto beforeAllSuite = Testament::detail::SuiteRegistry::instance().registerSuite(
        std::make_shared<Testament::detail::SuiteInstance>("before all suite")
    );
    beforeAllSuite->setBeforeSuite([] {
        throw std::runtime_error("suite setup failed");
    });
    beforeAllSuite->addTest(Testament::detail::RuntimeBridge::makeTest("first skipped test", {}, [] {}));
    beforeAllSuite->addTest(Testament::detail::RuntimeBridge::makeTest("second skipped test", {}, [] {}));

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
        && xml.contains("<testsuites tests=\"8\" failures=\"1\" errors=\"2\" skipped=\"2\"")
        && xml.contains("<testsuite name=\"suite &lt;&amp;&gt;\xef\xbf\xbd\"")
        && xml.contains("name=\"failing &quot;test&quot;\"")
        && xml.contains(
            std::string{"name=\"passing test\" file=\""} + __FILE__
            + "\" line=\"" + std::to_string(passingTestLine) + "\""
        )
        && xml.contains("failure &lt;reason&gt; &amp; details\xef\xbf\xbd\xef\xbf\xbd(")
        && !xml.contains('\x01')
        && xml.contains("<error message=\"Error in beforeEach: setup &lt;failed&gt;\"")
        && xml.contains("<testsuite name=\"before all suite\" tests=\"3\" failures=\"0\" errors=\"1\" skipped=\"2\"")
        && xml.contains("name=\"first skipped test\"")
        && xml.contains("name=\"second skipped test\"")
        && xml.contains("<error message=\"Error in beforeSuite: suite setup failed\"")
        && xml.contains("name=\"values / one\"")
        && xml.contains("name=\"values / two\"")
        ? 0
        : 1;
}
