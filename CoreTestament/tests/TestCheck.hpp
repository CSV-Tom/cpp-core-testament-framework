#ifndef TESTAMENT_TESTS_TESTCHECK_HPP
#define TESTAMENT_TESTS_TESTCHECK_HPP

#include <iostream>
#include <string_view>

namespace Testament::TestSupport {

class Checks {
public:
    void expect(bool condition, std::string_view message) {
        if (condition) return;
        succeeded = false;
        std::cerr << "CHECK FAILED: " << message << '\n';
    }

    [[nodiscard]] int result() const noexcept { return succeeded ? 0 : 1; }

private:
    bool succeeded{true};
};

}

#endif
