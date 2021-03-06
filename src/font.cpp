#include <array>
#include <vector>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <protowork/font.hpp>

namespace pw = protowork;

std::string get_default_font_path() {
    std::array<char, 128> buffer;
    std::string stdout;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen("fc-match -v monospace | grep file:", "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        stdout += buffer.data();
    }
    std::string result;
    bool is_in_filepath = false;
    for (auto c : stdout) {
        if (is_in_filepath && c != '\"') {
            result += c;
        }
        if (c == '\"')
            is_in_filepath = !is_in_filepath;
    }
    return result;
}

static const char *vertex_shader_code = R"(
#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 i_Position_screenspace;
layout(location = 1) in vec2 i_UV;

out vec2 UV;

uniform vec2 u_Size;

void main(){
    // map (0, 0) -> (-1, -1)
    //     (size.x, size.y) -> (1, 1)
    vec2 pos = 2 * i_Position_screenspace / u_Size - vec2(1, 1);
    gl_Position = vec4(pos, 0, 1);

    // UV of the vertex. No special space for this one.
    UV = i_UV;
})";

static const char *fragment_shader_code = R"(
#version 430 core

in vec2 UV;

out vec4 o_Color;

uniform sampler2D u_TextureSampler;

void main() {
    o_Color = texture(u_TextureSampler, UV);
})";

static id_t g_shader_id;
static id_t g_texture_sampler_id;
static id_t g_vertex_buffer_id;
static id_t g_uv_buffer_id;
static FT_Library g_library;
static FT_Face g_face;

bool pw::font::operator==(pw::font::key_t const &lhs,
                          pw::font::key_t const &rhs) {
    return lhs.font_size == rhs.font_size;
}

struct key_hash_t {
    size_t operator()(pw::font::key_t const &key) const {
        return std::hash<int>()(key.font_size);
    }
};

static std::unordered_map<pw::font::key_t, pw::font::data_t, key_hash_t>
    g_font_data;

void pw::font::initialize() {
    auto error = FT_Init_FreeType(&g_library);
    if (error) {
        throw std::runtime_error{"failed to initialize freetype2"};
    }

    auto font_path = get_default_font_path();
    error = FT_New_Face(g_library, font_path.c_str(), 0, &g_face);
    if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error{
            "freetype2 dones not support default font format: " + font_path};
    } else if (error) {
        throw std::runtime_error{"failed to load font file"};
    }

    g_shader_id =
        detail::load_shader_program(vertex_shader_code, fragment_shader_code);
    g_texture_sampler_id =
        glGetUniformLocation(g_shader_id, "u_TextureSampler");

    glGenBuffers(1, &g_vertex_buffer_id);
    glGenBuffers(1, &g_uv_buffer_id);
}

void pw::font::finalize() {
    for (auto const &[_, data] : g_font_data) {
        glDeleteTextures(1, &data.texture_id);
    }
    glDeleteBuffers(1, &g_vertex_buffer_id);
    glDeleteBuffers(1, &g_uv_buffer_id);
    glDeleteProgram(g_shader_id);
}

void pw::font::before_drawing() { glUseProgram(g_shader_id); }

void pw::font::render(GLFWwindow *window, int font_size,
                      std::vector<glm::vec2> const &vertices,
                      std::vector<glm::vec2> const &uvs) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  pw::font::get(font::key_t{font_size}).texture_id);
    glUniform1i(g_texture_sampler_id, 0);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glUniform2f(glGetUniformLocation(g_shader_id, "u_Size"), (float)width,
                (float)height);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, g_uv_buffer_id);
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

pw::font::data_t const &pw::font::get(pw::font::key_t const &key) {
    auto found = g_font_data.find(key);
    if (found == g_font_data.end()) {
        FT_Set_Pixel_Sizes(g_face, 0, key.font_size);

        data_t data;

        // calc text texture size
        int w = 0;
        int h = 0;
        for (int i = 32; i < 128; i++) {
            if (FT_Load_Char(g_face, i, FT_LOAD_RENDER)) {
                throw std::runtime_error{"failed to load charactor: " +
                                         std::string{(char)i}};
            }
            auto const &bitmap = g_face->glyph->bitmap;
            w += bitmap.width;
            h = std::max(h, (int)bitmap.rows);
        }
        data.atlas_width = w;
        data.atlas_height = h;

        // create empty texture with width x height
        glGenTextures(1, &data.texture_id);
        glBindTexture(GL_TEXTURE_2D, data.texture_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // write font glyph bitmap to texture
        int x = 0;
        for (int i = 32; i < 128; i++) {
            if (FT_Load_Char(g_face, i, FT_LOAD_RENDER))
                continue;
            auto glyph = g_face->glyph;
            int glyph_w = glyph->bitmap.width;
            int glyph_h = glyph->bitmap.rows;
            std::vector<GLubyte> buf(glyph_w * glyph_h * 4);
            for (int i = 0; i < glyph_h * glyph_w; i++) {
                buf[i * 4 + 0] = glyph->bitmap.buffer[i];
                buf[i * 4 + 1] = glyph->bitmap.buffer[i];
                buf[i * 4 + 2] = glyph->bitmap.buffer[i];
                buf[i * 4 + 3] = glyph->bitmap.buffer[i];
            }
            glTextureSubImage2D(data.texture_id, 0, x, 0, glyph->bitmap.width,
                                glyph->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE,
                                buf.data());

            data.char_infos[i].advance_x = glyph->advance.x >> 6;
            data.char_infos[i].width = glyph->bitmap.width;
            data.char_infos[i].height = glyph->bitmap.rows;
            data.char_infos[i].bearing_x = glyph->metrics.horiBearingX >> 6;
            data.char_infos[i].bearing_y = glyph->metrics.horiBearingY >> 6;
            data.char_infos[i].texture_x = x;
            data.char_infos[i].texture_y = 0;

            x += glyph->bitmap.width;
        }
        g_font_data.insert(std::make_pair(key, data));
        return g_font_data[key];
    } else {
        return found->second;
    }
}
