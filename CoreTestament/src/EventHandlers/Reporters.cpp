#include "Testament/Reporters.hpp"

#include "ConsoleTestEventHandler.hpp"
#include "JUnitTestEventHandler.hpp"

#include <filesystem>
#include <memory>
#include <utility>

namespace Testament {

std::unique_ptr<TestEventHandler> makeConsoleHandler() {
    return std::make_unique<detail::ConsoleTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler() {
    return std::make_unique<detail::JUnitTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath) {
    return std::make_unique<detail::JUnitTestEventHandler>(std::move(outputPath));
}

}
