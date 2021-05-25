#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <protowork.hpp>

using namespace protowork::detail;

// TODO

camera_t::camera_t() {
    m_last_time = glfwGetTime();
}

void camera_t::update(GLFWwindow* window) {
    double current_time = glfwGetTime();
    float delta_time = float(current_time - m_last_time);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetCursorPos(window, width/2, height/2);

    constexpr float MOUSE_SPEED = 0.005f;
    
    m_horizontal_angle += MOUSE_SPEED * float(width/2 - xpos );
    m_vertical_angle   += MOUSE_SPEED * float(height/2 - ypos );
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction {
        cos(m_vertical_angle) * sin(m_horizontal_angle), 
        sin(m_vertical_angle),
        cos(m_vertical_angle) * cos(m_horizontal_angle)
    };
    
    glm::vec3 right = glm::vec3 {
        sin(m_horizontal_angle - std::numbers::pi_v<float>/2.0f), 
        0,
        cos(m_horizontal_angle - std::numbers::pi_v<float>/2.0f)
    };

    constexpr float SPEED = 3.0f;

    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS)
        m_pos += direction * delta_time * SPEED;
    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS)
        m_pos -= direction * delta_time * SPEED;
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
        m_pos += right * delta_time * SPEED;
    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS)
        m_pos -= right * delta_time * SPEED;

    m_last_time = current_time;
}

glm::mat4 camera_t::projection() const {
    float constexpr FOV = 45.0f;
    return glm::perspective(glm::radians(FOV), 4.0f / 3.0f, 0.1f, 100.0f);
}

glm::mat4 camera_t::view() const {
    using namespace std;
    glm::vec3 direction {
        cos(m_vertical_angle) * sin(m_horizontal_angle), 
        sin(m_vertical_angle),
        cos(m_vertical_angle) * cos(m_horizontal_angle)
    };
    glm::vec3 right = glm::vec3 {
        sin(m_horizontal_angle - numbers::pi_v<float>/2.0f), 
        0,
        cos(m_horizontal_angle - numbers::pi_v<float>/2.0f)
    };
    glm::vec3 up = glm::cross(right, direction);

    return glm::lookAt(
        m_pos,
        m_pos + direction,
        up
    );
}
