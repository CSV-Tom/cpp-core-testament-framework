#include "JUnitTestEventHandler.hpp"

#include <exception>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string_view>
#include <system_error>
#include <utility>

namespace {

std::string escapeXml(std::string_view value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (const char character : value) {
        switch (character) {
        case '&':
            escaped += "&amp;";
            break;
        case '<':
            escaped += "&lt;";
            break;
        case '>':
            escaped += "&gt;";
            break;
        case '\"':
            escaped += "&quot;";
            break;
        case '\'':
            escaped += "&apos;";
            break;
        default:
            escaped += character;
        }
    }
    return escaped;
}

std::string exceptionMessage(const std::exception_ptr& exception) {
    if (!exception) {
        return "Unknown test failure";
    }
    try {
        std::rethrow_exception(exception);
    } catch (const std::exception& error) {
        return error.what();
    } catch (...) {
        return "Unknown non-standard exception";
    }
}

double durationSeconds(const Testament::TestEventHandler::TestInfo& test) {
    return test.duration.count();
}

}

JUnitTestEventHandler::JUnitTestEventHandler(std::filesystem::path outputPath_)
    : outputPath(std::move(outputPath_)) {}

void JUnitTestEventHandler::onStartReport(unsigned int suiteCount) {
    suiteResults.clear();
    suiteResults.reserve(suiteCount);
    reportWritten = false;
    writeError.clear();
}

void JUnitTestEventHandler::onSuiteStart(const SuiteInfo& suite) {
    suiteResults.push_back({suite.name, {}, {}});
}

void JUnitTestEventHandler::onSuiteAbort(const SuiteInfo& suite, const std::string& message) {
    suiteResult(suite).lifecycleErrors.push_back(message);
}

void JUnitTestEventHandler::onTestPassed(const SuiteInfo& suite, const TestInfo& test) {
    suiteResult(suite).tests.push_back({test, Status::Passed, {}});
}

void JUnitTestEventHandler::onTestFailed(const SuiteInfo& suite, const TestInfo& test) {
    suiteResult(suite).tests.push_back({test, Status::Failed, exceptionMessage(test.exception)});
}

void JUnitTestEventHandler::onTestSkipped(const SuiteInfo& suite, const TestInfo& test) {
    suiteResult(suite).tests.push_back({test, Status::Skipped, {}});
}

void JUnitTestEventHandler::onFinalReport(unsigned int, unsigned int, unsigned int, unsigned int) {
    writeReport();
}

bool JUnitTestEventHandler::writeSucceeded() const noexcept {
    return reportWritten;
}

std::string JUnitTestEventHandler::errorMessage() const {
    return writeError;
}

JUnitTestEventHandler::SuiteResult& JUnitTestEventHandler::suiteResult(const SuiteInfo& suite) {
    if (suiteResults.empty() || suiteResults.back().name != suite.name) {
        suiteResults.push_back({suite.name, {}, {}});
    }
    return suiteResults.back();
}

void JUnitTestEventHandler::writeReport() {
    std::size_t testCount = 0;
    std::size_t failureCount = 0;
    std::size_t skippedCount = 0;
    std::size_t errorCount = 0;
    double totalDuration = 0.0;

    for (const auto& suite : suiteResults) {
        testCount += suite.tests.size() + suite.lifecycleErrors.size();
        errorCount += suite.lifecycleErrors.size();
        for (const auto& test : suite.tests) {
            totalDuration += durationSeconds(test.info);
            failureCount += test.status == Status::Failed ? 1U : 0U;
            skippedCount += test.status == Status::Skipped ? 1U : 0U;
        }
    }

    std::error_code directoryError;
    if (const auto parent = outputPath.parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent, directoryError);
    }
    if (directoryError) {
        writeError = "Cannot create JUnit output directory: " + directoryError.message();
        return;
    }

    std::ofstream output(outputPath);
    if (!output) {
        writeError = "Cannot open JUnit output file: " + outputPath.string();
        return;
    }

    output << std::fixed << std::setprecision(9);
    output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    output << "<testsuites tests=\"" << testCount
           << "\" failures=\"" << failureCount
           << "\" errors=\"" << errorCount
           << "\" skipped=\"" << skippedCount
           << "\" time=\"" << totalDuration << "\">\n";

    for (const auto& suite : suiteResults) {
        std::size_t suiteFailures = 0;
        std::size_t suiteSkipped = 0;
        double suiteDuration = 0.0;
        for (const auto& test : suite.tests) {
            suiteDuration += durationSeconds(test.info);
            suiteFailures += test.status == Status::Failed ? 1U : 0U;
            suiteSkipped += test.status == Status::Skipped ? 1U : 0U;
        }

        output << "  <testsuite name=\"" << escapeXml(suite.name)
               << "\" tests=\"" << suite.tests.size() + suite.lifecycleErrors.size()
               << "\" failures=\"" << suiteFailures
               << "\" errors=\"" << suite.lifecycleErrors.size()
               << "\" skipped=\"" << suiteSkipped
               << "\" time=\"" << suiteDuration << "\">\n";

        for (const auto& test : suite.tests) {
            output << "    <testcase classname=\"" << escapeXml(suite.name)
                   << "\" name=\"" << escapeXml(test.info.name)
                   << "\" time=\"" << durationSeconds(test.info) << "\"";
            if (test.status == Status::Passed) {
                output << "/>\n";
            } else if (test.status == Status::Skipped) {
                output << "><skipped/></testcase>\n";
            } else {
                output << "><failure message=\"" << escapeXml(test.failureMessage)
                       << "\">" << escapeXml(test.failureMessage)
                       << "</failure></testcase>\n";
            }
        }

        for (const auto& error : suite.lifecycleErrors) {
            output << "    <testcase classname=\"" << escapeXml(suite.name)
                   << "\" name=\"lifecycle\" time=\"0.000000000\"><error message=\""
                   << escapeXml(error) << "\">" << escapeXml(error)
                   << "</error></testcase>\n";
        }
        output << "  </testsuite>\n";
    }
    output << "</testsuites>\n";

    output.close();
    if (!output) {
        writeError = "Failed while writing JUnit output file: " + outputPath.string();
        return;
    }
    reportWritten = true;
}
