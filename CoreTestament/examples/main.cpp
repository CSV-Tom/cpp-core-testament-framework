// Shared entry point for all examples in this directory. Each example translation unit
// registers its own Suite at static-initialization time (see docs/guide.md, "Multiple
// translation units and linking") - this file only runs the resulting registry.
#include <Testament/Testament.hpp>

int main(int argc, char** argv) {
    return Testament::run(argc, argv);
}
