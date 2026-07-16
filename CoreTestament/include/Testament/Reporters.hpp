#ifndef TESTAMENT_REPORTERS_HPP
#define TESTAMENT_REPORTERS_HPP

#include "Testament/TestEventHandler.hpp"

#include <filesystem>
#include <memory>

namespace Testament {

std::unique_ptr<TestEventHandler> makeConsoleHandler();
std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath);

}

#endif
