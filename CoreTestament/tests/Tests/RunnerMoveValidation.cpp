#include "Testament/Testament.hpp"

#include <memory>
#include <utility>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    explicit RecordingHandler(unsigned int& reports_) : reports(reports_) {}

    void onFinalReport(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) override {
        ++reports;
    }

private:
    unsigned int& reports;
};

}

int main() {
    auto suite = Testament::Suite("runner move", Testament::Test("passes", [] {}));
    unsigned int transferredReports{};
    unsigned int moveConstructedSourceReports{};
    unsigned int moveAssignedSourceReports{};

    Testament::Runner original;
    original.addHandler(std::make_unique<RecordingHandler>(transferredReports));

    Testament::Runner moved{std::move(original)};
    original.addHandler(std::make_unique<RecordingHandler>(moveConstructedSourceReports));

    Testament::Runner assigned;
    assigned = std::move(moved);
    moved.addHandler(std::make_unique<RecordingHandler>(moveAssignedSourceReports));

    return suite
        && assigned.run(0, nullptr) == 0
        && original.run(0, nullptr) == 0
        && moved.run(0, nullptr) == 0
        && transferredReports == 1
        && moveConstructedSourceReports == 1
        && moveAssignedSourceReports == 1
        ? 0
        : 1;
}
