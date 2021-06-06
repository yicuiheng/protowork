#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#include <protowork.hpp>

using namespace protowork;

window_t::window_t(config_t const &config) {
    if (!glfwInit())
        throw std::runtime_error{"Failed to initialize GLFW"};

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(config.width, config.height, config.title,
                                nullptr, nullptr);
    if (m_window == nullptr) {
        glfwTerminate();
        throw std::runtime_error{"Failed to open GLFW window"};
    }
    glfwMakeContextCurrent(m_window);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error{"Failed to initialize GLEW"};
    }
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(m_window, config.width / 2, config.height / 2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glPointSize(10.0f);

    model_t::initialize();
    font::initialize();
}

window_t::~window_t() {
    font::finalize();
    model_t::finalize();
    glfwTerminate();
}

bool window_t::should_close() const {
    if (glfwWindowShouldClose(m_window) != 0)
        return true;
    return false;
}

void update_input(GLFWwindow *window, input_t &input) {
    glfwGetCursorPos(window, &input.mouse.x, &input.mouse.y);

    // as innter representation:
    //   button_t::LEFT == GLFW_MOUSE_BUTTON_LEFT
    //   button_t::RIGHT == GLFW_MOUSE_BUTTON_RIGHT
    //   button_t::MIDDLE == GLFW_MOUSE_BUTTON_MIDDLE
    using button_t = input_t::mouse_t::button_t;
    using button_state_t = input_t::mouse_t::button_state_t;
    auto &mouse = input.mouse;
    for (int button = button_t::LEFT; button < button_t::N_BUTTONS; button++) {
        auto prev = mouse.buttons[button];
        bool is_current_pressed =
            glfwGetMouseButton(window, button) == GLFW_PRESS;
        if (!is_current_pressed)
            mouse.buttons[button] = button_state_t::RELEASED;
        else if (prev == button_state_t::RELEASED)
            mouse.buttons[button] = button_state_t::PUSHED;
        else
            mouse.buttons[button] = button_state_t::PRESSED;
    }
}

void window_t::update() {
    update_input(m_window, m_input);
    m_camera.update(m_input);
}

void window_t::draw() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_t::before_drawing(m_camera);

    for (auto const &model : m_models) {
        model->draw();
    }

    font::before_drawing();

    for (auto const &text : m_2d_texts) {
        text->draw(m_window);
    }
    glm::mat4 MVP = m_camera.projection() * m_camera.view();
    for (auto const &text : m_3d_texts) {
        text->draw(m_window, MVP);
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
