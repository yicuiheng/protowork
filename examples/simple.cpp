#include <cassert>
#include <iostream>
#include <protowork.hpp>

namespace pw = protowork;

int main() {
    auto window =
        pw::window_t{pw::window_t::config_t{640, 480, "simple example window"}};

    while (!window.should_close()) {
        window.update();
        window.draw();
    }
}
