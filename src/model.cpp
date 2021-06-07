#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <protowork.hpp>

using namespace protowork;

static const char *vertex_shader_code = R"(
#version 430 core

layout(location = 0) in vec3 i_VertexPosition_modelspace;
layout(location = 1) in vec3 i_VertexNormal_modelspace;
layout(location = 2) in vec2 i_VertexCoord;

out vec3 Normal_worldspace;
out vec2 Coord;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;

void main(){
    gl_Position =  u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(i_VertexPosition_modelspace, 1);

    Normal_worldspace = i_VertexNormal_modelspace; // (u_ModelMatrix * vec4(i_VertexNormal_modelspace, 1)).xyz;
    Coord = i_VertexCoord;
})";

static const char *fragment_shader_code = R"(
#version 430 core

in vec3 Normal_worldspace;
in vec2 Coord;

out vec4 o_Color;

void main()
{
    vec3 LightColor = vec3(1,1,1);
    vec3 LightDirection_worldspace = vec3(-1, -1, -1);

    // Material properties
    vec3 MaterialDiffuseColor = vec3(1.0, 0.2, 0.2);

    vec3 n = normalize(Normal_worldspace);
    vec3 l = normalize(LightDirection_worldspace);
    float cosTheta = clamp(dot(n, l), 0, 1);
    o_Color =
        vec4(normalize(MaterialDiffuseColor * LightColor * cosTheta), 1);
})";

static id_t g_shader_id;
static id_t g_projection_matrix_id;
static id_t g_view_matrix_id;
static id_t g_model_matrix_id;

void model_t::initialize() {
    g_shader_id =
        detail::load_shader_program(vertex_shader_code, fragment_shader_code);

    g_projection_matrix_id =
        glGetUniformLocation(g_shader_id, "u_ProjectionMatrix");
    g_view_matrix_id = glGetUniformLocation(g_shader_id, "u_ViewMatrix");
    g_model_matrix_id = glGetUniformLocation(g_shader_id, "u_ModelMatrix");
}

void model_t::finalize() { glDeleteProgram(g_shader_id); }

void model_t::before_drawing(camera_t const &camera) {
    glUseProgram(g_shader_id);

    glm::mat4 projection_matrix = camera.projection();
    glm::mat4 view_matrix = camera.view();

    glUniformMatrix4fv(g_projection_matrix_id, 1, GL_FALSE,
                       &projection_matrix[0][0]);
    glUniformMatrix4fv(g_view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);
}

model_t::model_t() {
    glGenVertexArrays(1, &m_vertex_array_id);
    glBindVertexArray(m_vertex_array_id);

    glGenBuffers(1, &m_vbo_vertex_buffer_id);
    glGenBuffers(1, &m_vbo_normal_buffer_id);
    glGenBuffers(1, &m_vbo_index_buffer_id);
}

model_t::~model_t() {
    glDeleteVertexArrays(1, &m_vertex_array_id);
    glDeleteBuffers(1, &m_vbo_vertex_buffer_id);
    glDeleteBuffers(1, &m_vbo_normal_buffer_id);
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

    auto const &normal_buffer = this->vbo_normal_buffer();
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normal_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, normal_buffer.size() * sizeof(pos_t),
                 normal_buffer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    auto const &index_buffer = this->vbo_index_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer.size() * sizeof(index_t),
                 index_buffer.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, index_buffer.size(), GL_UNSIGNED_SHORT,
                   nullptr);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}