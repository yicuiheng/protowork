#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <protowork/world/camera.hpp>
#include <protowork/input.hpp>

using namespace protowork::world;

camera_t::camera_t() { m_orientation = glm::quat{0.f, 0.f, 0.f, 1.f}; }

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

    float diff_x = input.mouse.x - prev_mouse_x;
    float diff_y = input.mouse.y - prev_mouse_y;

    if (is_left && !is_right && !is_middle) {
        // rotation around the target
        float yaw = diff_x / 100.f;
        float pitch = diff_y / 100.f;
        float roll = 0.f;
        glm::quat diff_orientation{glm::vec3{pitch, yaw, roll}};
        m_orientation *= diff_orientation;
        m_orientation = glm::normalize(m_orientation);
    } else if (!is_left && !is_right && is_middle) {
        // translation
        auto inv = glm::inverse(m_orientation);
        auto up = glm::vec3{0.f, 1.f, 0.f} * m_orientation;
        auto right = glm::vec3{1.f, 0.f, 0.f} * m_orientation;
        m_target_pos += (diff_x * 0.01f) * right;
        m_target_pos += -(diff_y * 0.01f) * up;
    }

    prev_mouse_x = input.mouse.x;
    prev_mouse_y = input.mouse.y;
}

glm::mat4 camera_t::projection() const {
    float constexpr FOV = 45.0f;
    return glm::perspective(glm::radians(FOV), 4.0f / 3.0f, 0.1f, 100.0f);
}

glm::mat4 camera_t::view() const {
    auto forward = glm::normalize(glm::vec3{0.f, 0.f, -1.f} * m_orientation);
    auto origin_pos = m_target_pos - m_distance * forward;
    glm::vec3 up, right;
    make_up_and_right_from_forward(up, right, forward);
    return glm::lookAt(origin_pos, m_target_pos, up);
}
