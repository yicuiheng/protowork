#ifndef PROTOWORK_INPUT_HPP
#define PROTOWORK_INPUT_HPP

namespace protowork {

struct input_t {
    struct mouse_t {
        enum button_t { LEFT = 0, RIGHT, MIDDLE, N_BUTTONS };
        enum class button_state_t { RELEASED, PRESSED, PUSHED };
        double x = 0.0;
        double y = 0.0;
        button_state_t buttons[button_t::N_BUTTONS] = {};
    } mouse;
};

} // namespace protowork

#endif
