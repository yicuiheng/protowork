#include <array>
#include <map>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <protowork.hpp>

using namespace protowork;

text2d_t::text2d_t(int x, int y, int font_size, std::string const &str)
    : m_x{x}, m_y{y}, m_font_size{font_size}, m_text{str} {
    auto const &font_data = font::get(font::key_t{font_size});
    int atlas_width = font_data.atlas_width;
    int atlas_height = font_data.atlas_height;
    for (unsigned int i = 0; i < m_text.size(); i++) {
        int c = m_text[i];
        auto const &info = font_data.char_infos.at(c);
        auto left = x + info.bearing_x;
        auto right = left + info.width;
        auto up = y + info.bearing_y;
        auto down = up - info.height;
        glm::vec2 vertex_up_left = glm::vec2(left, up);
        glm::vec2 vertex_up_right = glm::vec2(right, up);
        glm::vec2 vertex_down_right = glm::vec2(right, down);
        glm::vec2 vertex_down_left = glm::vec2(left, down);

        m_vertices.push_back(vertex_up_left);
        m_vertices.push_back(vertex_down_left);
        m_vertices.push_back(vertex_up_right);

        m_vertices.push_back(vertex_down_right);
        m_vertices.push_back(vertex_up_right);
        m_vertices.push_back(vertex_down_left);

        float uv_x = (float)info.texture_x / atlas_width;
        float uv_y = (float)info.texture_y / atlas_height;
        float uv_width = (float)info.width / atlas_width;
        float uv_height = (float)(info.texture_y + info.height) / atlas_height;

        x += info.advance_x;

        glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
        glm::vec2 uv_up_right = glm::vec2(uv_x + uv_width, uv_y);
        glm::vec2 uv_down_right = glm::vec2(uv_x + uv_width, uv_y + uv_height);
        glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + uv_height);

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

void text2d_t::draw(GLFWwindow *window) const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  font::get(font::key_t{m_font_size}).texture_id);
    glUniform1i(font::texture_sampler_id(), 0);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glUniform2f(font::size_id(), (float)width, (float)height);

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
