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
    suite->addTest(Testament::makeTest("passing test", [] {}));
    suite->addTest(Testament::makeTest("failing \"test\"", [] {
        throw std::runtime_error("failure <reason> & details");
    }));

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
        && xml.contains("<testsuites tests=\"2\" failures=\"1\" errors=\"0\" skipped=\"0\"")
        && xml.contains("<testsuite name=\"suite &lt;&amp;&gt;\"")
        && xml.contains("name=\"failing &quot;test&quot;\"")
        && xml.contains("failure &lt;reason&gt; &amp; details")
        ? 0
        : 1;
}
