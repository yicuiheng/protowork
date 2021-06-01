#include <array>
#include <map>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <protowork.hpp>

using namespace protowork;

text2d_t::text2d_t(int x, int y, std::string const &str)
    : m_x{x}, m_y{y}, m_text{str} {
    int font_size = 32;
    int atlas_width = font_manager_t::atlas_width();
    int atlas_height = font_manager_t::atlas_height();
    for (unsigned int i = 0; i < m_text.size(); i++) {
        int c = m_text[i];
        auto const &info = font_manager_t::char_info(c);
        glm::vec2 vertex_up_left = glm::vec2(x, y);
        glm::vec2 vertex_up_right = glm::vec2(x + font_size, y);
        glm::vec2 vertex_down_right = glm::vec2(x + font_size, y + font_size);
        glm::vec2 vertex_down_left = glm::vec2(x, y + font_size);

        m_vertices.push_back(vertex_up_left);
        m_vertices.push_back(vertex_down_left);
        m_vertices.push_back(vertex_up_right);

        m_vertices.push_back(vertex_down_right);
        m_vertices.push_back(vertex_up_right);
        m_vertices.push_back(vertex_down_left);

        float uv_x = (float)info.texture_x / atlas_width;
        float uv_y = (float)(info.texture_y + info.height) / atlas_height;
        float uv_width = (float)info.width / atlas_width;
        float uv_height = (float)(info.texture_y + info.height) / atlas_height;

        x += font_size;

        glm::vec2 uv_up_left = glm::vec2(uv_x, uv_height);
        glm::vec2 uv_up_right = glm::vec2(uv_x + uv_width, uv_height);
        glm::vec2 uv_down_right = glm::vec2(uv_x + uv_width, 0.f);
        glm::vec2 uv_down_left = glm::vec2(uv_x, 0.f);

        m_uvs.push_back(uv_up_left);
        m_uvs.push_back(uv_down_left);
        m_uvs.push_back(uv_up_right);

        m_uvs.push_back(uv_down_right);
        m_uvs.push_back(uv_up_right);
        m_uvs.push_back(uv_down_left);
    }
    glGenBuffers(1, &m_vertex_buffer_id);
    glGenBuffers(1, &m_uv_buffer_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec2),
                 m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2),
                 m_uvs.data(), GL_STATIC_DRAW);
}

void text2d_t::draw() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_manager_t::texture_id());
    glUniform1i(font_manager_t::texture_sampler_id(), 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

text2d_t::~text2d_t() {
    glDeleteBuffers(1, &m_vertex_buffer_id);
    glDeleteBuffers(1, &m_uv_buffer_id);
}
