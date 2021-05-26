#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <protowork.hpp>

using namespace protowork;

camera_t::camera_t() {
}

static double prev_mouse_x = 0.0;
static double prev_mouse_y = 0.0;
static double last_time = glfwGetTime();

void camera_t::rotate_around_target(input_t::mouse_t const& mouse) {
    auto diff = m_origin_pos - m_target_pos;
    double angle = atan2(diff.z, diff.x);
    angle += 0.01 * (mouse.x - prev_mouse_x);
    std::cout << angle << std::endl;
    double len = sqrt(diff.x * diff.x + diff.z * diff.z);
    m_origin_pos.x = m_target_pos.x + cos(angle) * len;
    m_origin_pos.z = m_target_pos.z + sin(angle) * len;
}

void camera_t::update(input_t const& input) {
    using button_t = input_t::mouse_t::button_t;
    using button_state_t = input_t::mouse_t::button_state_t;
    auto const& buttons = input.mouse.buttons;
    auto is_left = buttons[button_t::LEFT] != button_state_t::RELEASED;
    auto is_right = buttons[button_t::RIGHT] != button_state_t::RELEASED;
    auto is_middle = buttons[button_t::MIDDLE] != button_state_t::RELEASED;
    if (is_left && !is_right && !is_middle) {
        this->rotate_around_target(input.mouse);
    }
    double current_time = glfwGetTime();

    // TODO

    prev_mouse_x = input.mouse.x;
    prev_mouse_y = input.mouse.y;
    last_time = current_time;
}

glm::mat4 camera_t::projection() const {
    float constexpr FOV = 45.0f;
    return glm::perspective(glm::radians(FOV), 4.0f / 3.0f, 0.1f, 100.0f);
}

glm::mat4 camera_t::view() const {
    return glm::lookAt(
        m_origin_pos,
        m_target_pos,
        m_up
    );
}
