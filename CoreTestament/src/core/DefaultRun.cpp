#include "Testament/Runner.hpp"

#include "Testament/Reporters.hpp"
#include "Testament/TestEventHandler.hpp"

namespace Testament {

int run(int argc, char** argv) {
    Runner runner;
    runner.addHandler(makeConsoleHandler());
    runner.addHandler(makeJUnitHandler());
    return runner.run(argc, argv);
}

}
