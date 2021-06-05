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

text3d_t::text3d_t(GLFWwindow *window, glm::vec3 pos, int font_size,
                   std::string const &str)
    : m_pos{pos}, m_font_size{font_size}, m_text{str} {

    glGenBuffers(1, &m_vertex_buffer_id);
    glGenBuffers(1, &m_uv_buffer_id);
}

void text3d_t::draw(GLFWwindow *window) {
    m_vertices.clear();
    m_uvs.clear();
    auto const &font_data = font::get(font::key_t{m_font_size});
    int atlas_width = font_data.atlas_width;
    int atlas_height = font_data.atlas_height;
    int x = 0;
    int y = 0;
    int screen_width, screen_height;
    glfwGetWindowSize(window, &screen_width, &screen_height);
    for (unsigned int i = 0; i < m_text.size(); i++) {
        int c = m_text[i];
        auto const &info = font_data.char_infos.at(c);
        auto left_pixel = x + info.bearing_x;
        auto right_pixel = left_pixel + info.width;
        auto up_pixel = y + info.bearing_y;
        auto down_pixel = up_pixel - info.height;
        float left = m_pos.x + 10.f * (float)left_pixel / screen_width;
        float right = m_pos.x + 10.f * (float)right_pixel / screen_width;
        float up = m_pos.y + 10.f * (float)up_pixel / screen_height;
        float down = m_pos.y + 10.f * (float)down_pixel / screen_height;
        glm::vec3 vertex_up_left{left, up, m_pos.z};
        glm::vec3 vertex_up_right{right, up, m_pos.z};
        glm::vec3 vertex_down_right{right, down, m_pos.z};
        glm::vec3 vertex_down_left{left, down, m_pos.z};

        m_vertices.push_back(vertex_up_left);
        m_vertices.push_back(vertex_down_left);
        m_vertices.push_back(vertex_up_right);

        m_vertices.push_back(vertex_down_right);
        m_vertices.push_back(vertex_up_right);
        m_vertices.push_back(vertex_down_left);

        x += info.advance_x;

        float uv_x = (float)info.texture_x / atlas_width;
        float uv_y = (float)info.texture_y / atlas_height;
        float uv_width = (float)info.width / atlas_width;
        float uv_height = (float)info.height / atlas_height;

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

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3),
                 m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2),
                 m_uvs.data(), GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  font::get(font::key_t{m_font_size}).texture_id);
    glUniform1i(font::text3d_texture_sampler_id(), 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

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

text3d_t::~text3d_t() {
    glDeleteBuffers(1, &m_vertex_buffer_id);
    glDeleteBuffers(1, &m_uv_buffer_id);
}
