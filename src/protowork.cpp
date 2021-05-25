#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <protowork.hpp>

using namespace protowork;

window_t::window_t(config_t const& config) {
    if (!glfwInit())
        throw std::runtime_error{"Failed to initialize GLFW"};

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
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
    glfwSetCursorPos(m_window, config.width/2, config.height/2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glPointSize(10.0f);

    m_shader_id = detail::load_shader_program();

    m_mvp_matrix_id = glGetUniformLocation(m_shader_id, "MVP");
    m_view_matrix_id = glGetUniformLocation(m_shader_id, "V");
    m_model_matrix_id = glGetUniformLocation(m_shader_id, "M");
    m_light_id = glGetUniformLocation(m_shader_id, "LightPosition_worldspace");
}

window_t::~window_t() {
    glDeleteProgram(m_shader_id);
    glfwTerminate();
}

bool window_t::should_close() const {
    if (glfwWindowShouldClose(m_window) != 0)
        return true;
    return false;
}

void window_t::update() {
    m_camera.update(m_window);
}

void window_t::draw() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader_id);

    glm::mat4 projection_matrix = m_camera.projection();
    glm::mat4 view_matrix = m_camera.view();
    glm::mat4 model_matrix = glm::mat4(1.0f);
    glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;

    glUniformMatrix4fv(m_mvp_matrix_id, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(m_model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
    glUniformMatrix4fv(m_view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);

    glm::vec3 lightPos = glm::vec3(4.0f,4.0f,-1.0f);
    glUniform3f(m_light_id, lightPos.x, lightPos.y, lightPos.z);

    for (auto const& model : m_models) {
        model->draw();
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
