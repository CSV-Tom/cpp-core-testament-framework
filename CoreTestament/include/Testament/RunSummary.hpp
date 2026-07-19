#ifndef TESTAMENT_RUNSUMMARY_HPP
#define TESTAMENT_RUNSUMMARY_HPP

namespace Testament {

struct RunSummary {
    unsigned int suites{};
    unsigned int passed{};
    unsigned int failed{};
    unsigned int skipped{};
    unsigned int errors{};
    unsigned int environmentErrors{};
    unsigned int repetitions{1};
};

}

#endif
