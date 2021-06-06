#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <protowork.hpp>

using namespace protowork;

static const char *vertex_shader_code = R"(
#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexCoord;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 LightDirection_cameraspace;
out vec2 Coord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

void main(){
    gl_Position =  P * V * M * vec4(vertexPosition_modelspace, 1);

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

static id_t g_shader_id;
static id_t g_projection_matrix_id;
static id_t g_view_matrix_id;
static id_t g_model_matrix_id;
static id_t g_light_id;

void model_t::initialize() {
    g_shader_id =
        detail::load_shader_program(vertex_shader_code, fragment_shader_code);

    g_projection_matrix_id = glGetUniformLocation(g_shader_id, "P");
    g_view_matrix_id = glGetUniformLocation(g_shader_id, "V");
    g_model_matrix_id = glGetUniformLocation(g_shader_id, "M");
    g_light_id = glGetUniformLocation(g_shader_id, "LightPosition_worldspace");
}

void model_t::finalize() { glDeleteProgram(g_shader_id); }

void model_t::before_drawing(camera_t const &camera) {
    glUseProgram(g_shader_id);

    glm::mat4 projection_matrix = camera.projection();
    glm::mat4 view_matrix = camera.view();

    glUniformMatrix4fv(g_projection_matrix_id, 1, GL_FALSE,
                       &projection_matrix[0][0]);
    glUniformMatrix4fv(g_view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);

    glm::vec3 lightPos = glm::vec3(4.0f, 4.0f, -1.0f);
    glUniform3f(g_light_id, lightPos.x, lightPos.y, lightPos.z);
}

model_t::model_t() {
    glGenVertexArrays(1, &m_vertex_array_id);
    glBindVertexArray(m_vertex_array_id);

    glGenBuffers(1, &m_vbo_vertex_buffer_id);
    glGenBuffers(1, &m_vbo_index_buffer_id);
}

model_t::~model_t() {
    glDeleteVertexArrays(1, &m_vertex_array_id);
    glDeleteBuffers(1, &m_vbo_vertex_buffer_id);
    glDeleteBuffers(1, &m_vbo_index_buffer_id);
}

void model_t::draw() const {
    auto model_matrix = glm::mat4(1.f);
    glUniformMatrix4fv(g_model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);

    auto const &vertex_buffer = this->vbo_vertex_buffer();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(pos_t),
                 vertex_buffer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    auto const &index_buffer = this->vbo_index_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer.size() * sizeof(index_t),
                 index_buffer.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, index_buffer.size(), GL_UNSIGNED_SHORT,
                   nullptr);

    glDisableVertexAttribArray(0);
}