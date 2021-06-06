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

static void draw_impl(GLFWwindow *window, int x, int y, int font_size,
                      std::string const &text) {
    auto const &font_data = font::get(font::key_t{font_size});
    int atlas_width = font_data.atlas_width;
    int atlas_height = font_data.atlas_height;
    std::vector<glm::vec2> vertices, uvs;
    for (unsigned int i = 0; i < text.size(); i++) {
        int c = text[i];
        auto const &info = font_data.char_infos.at(c);
        auto left = x + info.bearing_x;
        auto right = left + info.width;
        auto up = y + info.bearing_y;
        auto down = up - info.height;
        glm::vec2 vertex_up_left = glm::vec2(left, up);
        glm::vec2 vertex_up_right = glm::vec2(right, up);
        glm::vec2 vertex_down_right = glm::vec2(right, down);
        glm::vec2 vertex_down_left = glm::vec2(left, down);

        vertices.push_back(vertex_up_left);
        vertices.push_back(vertex_down_left);
        vertices.push_back(vertex_up_right);

        vertices.push_back(vertex_down_right);
        vertices.push_back(vertex_up_right);
        vertices.push_back(vertex_down_left);

        float uv_x = (float)info.texture_x / atlas_width;
        float uv_y = (float)info.texture_y / atlas_height;
        float uv_width = (float)info.width / atlas_width;
        float uv_height = (float)info.height / atlas_height;

        x += info.advance_x;

        glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
        glm::vec2 uv_up_right = glm::vec2(uv_x + uv_width, uv_y);
        glm::vec2 uv_down_right = glm::vec2(uv_x + uv_width, uv_y + uv_height);
        glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + uv_height);

        uvs.push_back(uv_up_left);
        uvs.push_back(uv_down_left);
        uvs.push_back(uv_up_right);

        uvs.push_back(uv_down_right);
        uvs.push_back(uv_up_right);
        uvs.push_back(uv_down_left);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font::get(font::key_t{font_size}).texture_id);
    glUniform1i(font::texture_sampler_id(), 0);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glUniform2f(font::size_id(), (float)width, (float)height);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, font::vertex_buffer_id());
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, font::uv_buffer_id());
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void text2d_t::draw(GLFWwindow *window) const {
    draw_impl(window, x, y, font_size, text);
}

void text3d_t::draw(GLFWwindow *window, glm::mat4 const &mat) const {
    int screen_width, screen_height;
    glfwGetWindowSize(window, &screen_width, &screen_height);
    auto pos = mat * glm::vec4{this->pos, 1.f};

    int x = (pos.x / pos.w + 1.f) * screen_width / 2.f;
    int y = (pos.y / pos.w + 1.f) * screen_height / 2.f;
    draw_impl(window, x, y, font_size, text);
}
