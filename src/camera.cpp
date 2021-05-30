#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <protowork.hpp>

using namespace protowork;

camera_t::camera_t() {}

static double prev_mouse_x = 0.0;
static double prev_mouse_y = 0.0;

void make_up_and_right_from_forward(glm::vec3 &up, glm::vec3 &right,
                                    glm::vec3 const &forward) {
    right = glm::cross(forward, glm::vec3{0.f, 1.f, 0.f});
    up = glm::cross(right, forward);
}

void camera_t::update(input_t const &input) {
    using button_t = input_t::mouse_t::button_t;
    using button_state_t = input_t::mouse_t::button_state_t;
    auto const &buttons = input.mouse.buttons;
    auto is_left = buttons[button_t::LEFT] != button_state_t::RELEASED;
    auto is_right = buttons[button_t::RIGHT] != button_state_t::RELEASED;
    auto is_middle = buttons[button_t::MIDDLE] != button_state_t::RELEASED;

    auto forward = glm::normalize(m_target_pos - m_origin_pos);
    glm::vec3 up, right;
    make_up_and_right_from_forward(up, right, forward);

    float diff_x = input.mouse.x - prev_mouse_x;
    float diff_y = input.mouse.y - prev_mouse_y;

    if (is_left || is_middle) {
        auto len = glm::length(m_target_pos - m_origin_pos);
        m_origin_pos += -(diff_x * 0.1f) * right;
        m_origin_pos += (diff_y * 0.1f) * up;
        m_origin_pos = glm::normalize(m_origin_pos) * len;
    }
    if (is_middle) {
        m_origin_pos += -(diff_x * 0.1f) * right;
        m_origin_pos += (diff_y * 0.1f) * up;
        m_target_pos += -(diff_x * 0.1f) * right;
        m_target_pos += (diff_y * 0.1f) * up;
    }

    // TODO

    prev_mouse_x = input.mouse.x;
    prev_mouse_y = input.mouse.y;
}

glm::mat4 camera_t::projection() const {
    float constexpr FOV = 45.0f;
    return glm::perspective(glm::radians(FOV), 4.0f / 3.0f, 0.1f, 100.0f);
}

glm::mat4 camera_t::view() const {
    auto forward = glm::normalize(m_target_pos - m_origin_pos);
    glm::vec3 up, right;
    make_up_and_right_from_forward(up, right, forward);
    return glm::lookAt(m_origin_pos, m_target_pos, up);
}
