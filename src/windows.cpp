#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#include <protowork.hpp>

static const char *vertex_shader_code = R"(
#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexCoord;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 LightDirection_cameraspace;
out vec2 Coord;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

void main(){
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

    Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
    vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

    vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;

    Coord = vertexCoord;
})";

static const char *fragment_shader_code = R"(
#version 430 core

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec2 Coord;

out vec3 color;

uniform vec3 LightPosition_worldspace;

vec3 calcColor(vec2 coord,float n) {
    vec3 color1 = vec3(1.0, 0.2, 0.2);
    vec3 color2 = vec3(0.9, 0.8, 0.8);

    coord = coord * n;
    float a = mod(floor(coord.x) + floor(coord.y), 2.0);
    return mix(color1, color2, a);
}

void main()
{
    vec3 LightColor = vec3(1,1,1);
    float LightPower = 50.0f;

    // Material properties
    vec3 MaterialDiffuseColor = calcColor(Coord, 5.0);
    vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;

    float distance = length(LightPosition_worldspace - Position_worldspace);

    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);
    float cosTheta = clamp(dot(n, l), 0, 1);

    color =
        MaterialAmbientColor +
        MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance);
}
)";

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

    m_shader_id =
        detail::load_shader_program(vertex_shader_code, fragment_shader_code);

    m_mvp_matrix_id = glGetUniformLocation(m_shader_id, "MVP");
    m_view_matrix_id = glGetUniformLocation(m_shader_id, "V");
    m_model_matrix_id = glGetUniformLocation(m_shader_id, "M");
    m_light_id = glGetUniformLocation(m_shader_id, "LightPosition_worldspace");

    font::initialize();
}

window_t::~window_t() {
    font::finalize();
    glDeleteProgram(m_shader_id);
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

    glUseProgram(m_shader_id);

    glm::mat4 projection_matrix = m_camera.projection();
    glm::mat4 view_matrix = m_camera.view();
    glm::mat4 model_matrix = glm::mat4(1.0f);
    glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;

    glUniformMatrix4fv(m_mvp_matrix_id, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(m_model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
    glUniformMatrix4fv(m_view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);

    glm::vec3 lightPos = glm::vec3(4.0f, 4.0f, -1.0f);
    glUniform3f(m_light_id, lightPos.x, lightPos.y, lightPos.z);

    for (auto const &model : m_models) {
        model->draw();
    }

    glUseProgram(font::shader_id());
    for (auto const &text : m_text_2ds) {
        text->draw();
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
