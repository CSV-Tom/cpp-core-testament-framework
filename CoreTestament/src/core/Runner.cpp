#include "Testament/Runner.hpp"

#include "Internal/InternalRegistry.hpp"
namespace Testament {

int Runner::run(int argc, char** argv) {

    auto& registry = InternalRegistry::getInstance();
    for (auto& suite : registry.getAllSuites()) {
        suite->run();
    }
    //TODO:
    //
    // TestRegistry::instance().run(argc, argv);
    return argc > 0 && argv != nullptr;
}

}