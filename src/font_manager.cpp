#include <array>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <protowork.hpp>

using namespace protowork;

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
layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

void main(){

    // Output position of the vertex, in clip space
    // map [0..800][0..600] to [-1..1][-1..1]
    vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace - vec2(400,300); // [0..800][0..600] -> [-400..400][-300..300]
    vertexPosition_homoneneousspace /= vec2(400,300);
    gl_Position = vec4(vertexPosition_homoneneousspace,0,1);

    // UV of the vertex. No special space for this one.
    UV = vertexUV;
})";

static const char *fragment_shader_code = R"(
#version 430 core

in vec2 UV;

out vec4 color;

uniform sampler2D myTextureSampler;

void main() {
    color = texture(myTextureSampler, UV);
    // color = vec4(1);
})";

static GLuint g_vertex_array_id;
static GLuint g_vertex_buffer_id;
static GLuint g_uv_buffer_id;
static GLuint g_shader_id;
static GLuint g_texture_id;
static GLuint g_texture_sampler_id;

static int g_atlas_width, g_atlas_height;
static font_manager_t::char_info_t g_char_infos[128];
static FT_Library g_library;
static FT_Face g_face;
static int g_font_size = 42;

void font_manager_t::initialize() {
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
    FT_Set_Pixel_Sizes(g_face, 0, g_font_size);

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
    g_atlas_width = w;
    g_atlas_height = h;

    // create empty texture with width x height
    glGenTextures(1, &g_texture_id);
    glBindTexture(GL_TEXTURE_2D, g_texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
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
        glTextureSubImage2D(g_texture_id, 0, x, 0, glyph->bitmap.width,
                            glyph->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE,
                            buf.data());

        g_char_infos[i].advance_x = glyph->advance.x >> 6;
        g_char_infos[i].width = glyph->bitmap.width;
        g_char_infos[i].height = glyph->bitmap.rows;
        g_char_infos[i].bearing_x = glyph->metrics.horiBearingX >> 6;
        g_char_infos[i].bearing_y = glyph->metrics.horiBearingY >> 6;
        g_char_infos[i].texture_x = x;
        g_char_infos[i].texture_y = 0;

        x += glyph->bitmap.width;
    }

    glGenVertexArrays(1, &g_vertex_array_id);
    glBindVertexArray(g_vertex_array_id);

    glGenBuffers(1, &g_vertex_buffer_id);
    glGenBuffers(1, &g_uv_buffer_id);

    g_shader_id =
        detail::load_shader_program(vertex_shader_code, fragment_shader_code);
    g_texture_sampler_id =
        glGetUniformLocation(g_shader_id, "myTextureSampler");
}

void protowork::print_text_2d(int x, int y, std::string const &text) {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;
    for (unsigned int i = 0; i < text.size(); i++) {
        int c = text[i];
        glm::vec2 vertex_up_left = glm::vec2(x, y);
        glm::vec2 vertex_up_right = glm::vec2(x + g_font_size, y);
        glm::vec2 vertex_down_right =
            glm::vec2(x + g_font_size, y + g_font_size);
        glm::vec2 vertex_down_left = glm::vec2(x, y + g_font_size);

        vertices.push_back(vertex_up_left);
        vertices.push_back(vertex_down_left);
        vertices.push_back(vertex_up_right);

        vertices.push_back(vertex_down_right);
        vertices.push_back(vertex_up_right);
        vertices.push_back(vertex_down_left);

        float uv_x = (float)g_char_infos[c].texture_x / g_atlas_width;
        float uv_y =
            (float)(g_char_infos[c].texture_y + g_char_infos[c].height) /
            g_atlas_height;
        float uv_width = (float)g_char_infos[c].width / g_atlas_width;
        float uv_height =
            (float)(g_char_infos[c].texture_y + g_char_infos[c].height) /
            g_atlas_height;

        x += g_font_size;

        glm::vec2 uv_up_left = glm::vec2(uv_x, uv_height);
        glm::vec2 uv_up_right = glm::vec2(uv_x + uv_width, uv_height);
        glm::vec2 uv_down_right = glm::vec2(uv_x + uv_width, 0.f);
        glm::vec2 uv_down_left = glm::vec2(uv_x, 0.f);

        UVs.push_back(uv_up_left);
        UVs.push_back(uv_down_left);
        UVs.push_back(uv_up_right);

        UVs.push_back(uv_down_right);
        UVs.push_back(uv_up_right);
        UVs.push_back(uv_down_left);
    }
    glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
                 &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, g_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0],
                 GL_STATIC_DRAW);

    // Bind shader
    glUseProgram(g_shader_id);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texture_id);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(g_texture_sampler_id, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, g_uv_buffer_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Draw call
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void font_manager_t::finalize() {
    glDeleteBuffers(1, &g_vertex_array_id);
    glDeleteBuffers(1, &g_vertex_buffer_id);
    glDeleteBuffers(1, &g_uv_buffer_id);
    glDeleteTextures(1, &g_texture_id);
    glDeleteProgram(g_shader_id);
}

font_manager_t::char_info_t const &font_manager_t::char_info(int c) {
    return g_char_infos[c];
}

GLuint font_manager_t::shader_id() { return g_shader_id; }

GLuint font_manager_t::texture_id() { return g_texture_id; }

GLuint font_manager_t::texture_sampler_id() { return g_texture_sampler_id; }

int font_manager_t::atlas_width() { return g_atlas_width; }

int font_manager_t::atlas_height() { return g_atlas_height; }
