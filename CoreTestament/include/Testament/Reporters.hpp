#ifndef TESTAMENT_REPORTERS_HPP
#define TESTAMENT_REPORTERS_HPP

#include "Testament/TestEventHandler.hpp"

#include <filesystem>
#include <memory>

namespace Testament {

std::unique_ptr<TestEventHandler> makeConsoleHandler();
// Creates a handler that is enabled and configured by the --junit command-line option.
std::unique_ptr<TestEventHandler> makeJUnitHandler();
std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath);

}

#endif
