#include "JUnitTestEventHandler.hpp"

#include <exception>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace {

struct DecodedCharacter {
    char32_t codePoint;
    std::size_t length;
};

std::optional<DecodedCharacter> decodeUtf8(std::string_view value, std::size_t offset) {
    const auto first = static_cast<unsigned char>(value[offset]);
    if (first <= 0x7f) return DecodedCharacter{first, 1};

    std::size_t length{};
    char32_t codePoint{};
    char32_t minimum{};
    if ((first & 0xe0) == 0xc0) {
        length = 2;
        codePoint = first & 0x1f;
        minimum = 0x80;
    } else if ((first & 0xf0) == 0xe0) {
        length = 3;
        codePoint = first & 0x0f;
        minimum = 0x800;
    } else if ((first & 0xf8) == 0xf0) {
        length = 4;
        codePoint = first & 0x07;
        minimum = 0x10000;
    } else {
        return std::nullopt;
    }

    if (offset + length > value.size()) return std::nullopt;
    for (std::size_t index = 1; index < length; ++index) {
        const auto continuation = static_cast<unsigned char>(value[offset + index]);
        if ((continuation & 0xc0) != 0x80) return std::nullopt;
        codePoint = (codePoint << 6) | (continuation & 0x3f);
    }

    if (codePoint < minimum || codePoint > 0x10ffff
        || (codePoint >= 0xd800 && codePoint <= 0xdfff)) {
        return std::nullopt;
    }
    return DecodedCharacter{codePoint, length};
}

bool isXmlCharacter(char32_t codePoint) {
    return codePoint == 0x09 || codePoint == 0x0a || codePoint == 0x0d
        || (codePoint >= 0x20 && codePoint <= 0xd7ff)
        || (codePoint >= 0xe000 && codePoint <= 0xfffd)
        || (codePoint >= 0x10000 && codePoint <= 0x10ffff);
}

std::string escapeXml(std::string_view value) {
    constexpr std::string_view replacementCharacter{"\xef\xbf\xbd"};
    std::string escaped;
    escaped.reserve(value.size());
    for (std::size_t offset = 0; offset < value.size();) {
        const auto decoded = decodeUtf8(value, offset);
        if (!decoded) {
            escaped += replacementCharacter;
            ++offset;
            continue;
        }
        if (!isXmlCharacter(decoded->codePoint)) {
            escaped += replacementCharacter;
            offset += decoded->length;
            continue;
        }

        switch (decoded->codePoint) {
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
            escaped += value.substr(offset, decoded->length);
        }
        offset += decoded->length;
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

void JUnitTestEventHandler::onSuiteAbort(const SuiteInfo& suite, std::string_view message) {
    suiteResult(suite).lifecycleErrors.emplace_back(message);
}

void JUnitTestEventHandler::onTestPassed(const SuiteInfo& suite, const TestInfo& test) {
    suiteResult(suite).tests.push_back({test, Status::Passed, {}});
}

void JUnitTestEventHandler::onTestFailed(const SuiteInfo& suite, const TestInfo& test) {
    const auto status = test.status == TestResultStatus::LifecycleError
        ? Status::Error
        : Status::Failed;
    suiteResult(suite).tests.push_back({test, status, exceptionMessage(test.exception)});
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
            errorCount += test.status == Status::Error ? 1U : 0U;
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
        std::size_t suiteErrors = suite.lifecycleErrors.size();
        std::size_t suiteSkipped = 0;
        double suiteDuration = 0.0;
        for (const auto& test : suite.tests) {
            suiteDuration += durationSeconds(test.info);
            suiteFailures += test.status == Status::Failed ? 1U : 0U;
            suiteErrors += test.status == Status::Error ? 1U : 0U;
            suiteSkipped += test.status == Status::Skipped ? 1U : 0U;
        }

        output << "  <testsuite name=\"" << escapeXml(suite.name)
               << "\" tests=\"" << suite.tests.size() + suite.lifecycleErrors.size()
               << "\" failures=\"" << suiteFailures
               << "\" errors=\"" << suiteErrors
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
            } else if (test.status == Status::Failed) {
                output << "><failure message=\"" << escapeXml(test.failureMessage)
                       << "\">" << escapeXml(test.failureMessage)
                       << "</failure></testcase>\n";
            } else {
                output << "><error message=\"" << escapeXml(test.failureMessage)
                       << "\">" << escapeXml(test.failureMessage)
                       << "</error></testcase>\n";
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
