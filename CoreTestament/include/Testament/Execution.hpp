#ifndef TESTAMENT_EXECUTION_HPP
#define TESTAMENT_EXECUTION_HPP

namespace Testament {

enum class Execution {
    // Follow the runner's configured concurrency limits.
    Inherit,
    // Execute exclusively with respect to adjacent work at the same level.
    Serial,
    // Explicitly permit parallel execution; the runner limit still applies.
    Parallel
};

}

#endif
