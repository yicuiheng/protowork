#include <array>
#include <map>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <protowork.hpp>
/*

using namespace protowork;

struct text2d_t {
    explicit text2d_t(int left, int top, std::string const &str);
    void draw_impl() const;
    virtual ~text2d_t();

private:
    std::string m_text;
    std::vector<glm::vec2> m_vertices;
    std::vector<glm::vec2> m_uvs;

    GLuint m_index_buffer_id;
};


text2d_t::text2d_t(int left, int top, std::string const &str) : m_text{str} {
    glGenVertexArrays(1, &m_vertex_array_id);
    glBindVertexArray(m_vertex_array_id);

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;
    int x = left;
    int y = top;
    for (int i = 0; m_text[i] != '\0'; i++) {
        int c = m_text[i];
        auto const &char_info = font_manager_t::char_info(c);
        {
            // set vertices position data
            int left_x = x + char_info.bearing_x;
            int right_x = left_x + char_info.width;
            int top_y = y + char_info.bearing_y;
            int bottom_y = top_y + char_info.height;

            m_vertices.emplace_back(left_x, top_y);
            m_vertices.emplace_back(left_x, bottom_y);
            m_vertices.emplace_back(right_x, top_y);

            m_vertices.emplace_back(right_x, bottom_y);
            m_vertices.emplace_back(right_x, top_y);
            m_vertices.emplace_back(left_x, bottom_y);

            x += char_info.advance_x;
        }

        {
            // set UV data
            GLfloat uv_x =
                (GLfloat)char_info.texture_x / font_manager_t::atlas_width();
            GLfloat uv_width =
                (GLfloat)char_info.width / font_manager_t::atlas_width();
            GLfloat uv_height =
                (GLfloat)char_info.height / font_manager_t::atlas_height();

            glm::vec2 upper_left{uv_x, uv_height};
            glm::vec2 upper_right{uv_x + uv_width, uv_height};
            glm::vec2 lower_left{uv_x, uv_height};
            glm::vec2 lower_right{uv_x + uv_width, uv_height};

            m_uvs.push_back(upper_left);
            m_uvs.push_back(lower_left);
            m_uvs.push_back(upper_right);

            m_uvs.push_back(lower_right);
            m_uvs.push_back(upper_right);
            m_uvs.push_back(lower_left);
        }
    }

    glGenBuffers(1, &m_vertex_buffer_id);
    glGenBuffers(1, &m_uv_buffer_id);
}

void text2d_t::draw_impl() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec2),
                 m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2),
                 m_uvs.data(), GLEW_STATIC);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_manager_t::texture_id());
    glUniform1i(font_manager_t::texture_sampler_id(), 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 2);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id);
    // glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

text2d_t::~text2d_t() {
    glDeleteBuffers(1, &m_vertex_array_id);
    glDeleteBuffers(1, &m_vertex_buffer_id);
    glDeleteBuffers(1, &m_uv_buffer_id);
    // glDeleteBuffers(1, &m_index_buffer_id);
}
*/