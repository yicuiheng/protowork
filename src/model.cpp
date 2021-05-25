#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <protowork.hpp>

using namespace protowork;

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
    auto const& vertex_buffer = this->vbo_vertex_buffer();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(pos_t), vertex_buffer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    auto const& index_buffer = this->vbo_index_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer.size() * sizeof(index_t), index_buffer.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, index_buffer.size(), GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(0);
}