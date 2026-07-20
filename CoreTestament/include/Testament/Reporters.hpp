#ifndef TESTAMENT_REPORTERS_HPP
#define TESTAMENT_REPORTERS_HPP

#include "Testament/Export.hpp"

#include <filesystem>
#include <memory>

namespace Testament {

class TestEventHandler;

TESTAMENT_EXPORT std::unique_ptr<TestEventHandler> makeConsoleHandler();
// Creates a handler that is enabled and configured by the --junit command-line option.
TESTAMENT_EXPORT std::unique_ptr<TestEventHandler> makeJUnitHandler();
TESTAMENT_EXPORT std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath);

}

#endif
