#include "Testament/Runner.hpp"

namespace Testament {

int Runner::run(int argc, char** argv) {
    //TODO:
    //
    // TestRegistry::instance().run(argc, argv);
    return argc > 0 && argv != nullptr;
}

}