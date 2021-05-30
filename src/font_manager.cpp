#include <array>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <protowork.hpp>

using namespace protowork;

std::string get_default_font_path() {
    std::array<char, 128> buffer;
    std::string stdout;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("fc-match -v monospace | grep file:", "r"), pclose);
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

static const char* vertex_shader_code = R"(
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
    gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);

    // UV of the vertex. No special space for this one.
    UV = vertexUV;
})";

static const char* fragment_shader_code = R"(
#version 430 core

in vec2 UV;

out vec4 color;

uniform sampler2D myTextureSampler;

void main() {
    color = texture(myTextureSampler, UV);
    // color = vec4(1);
})";

static GLuint g_shader_id;
static GLuint g_texture_id;
static GLuint g_texture_sampler_id;
static int g_atlas_width, g_atlas_height;
static font_manager_t::char_info_t g_char_infos[128];
static FT_Library g_library;
static FT_Face g_face;
static int g_font_size = 32;

void font_manager_t::initialize() {
    g_shader_id = detail::load_shader_program(vertex_shader_code, fragment_shader_code);

    auto error = FT_Init_FreeType(&g_library);
    if (error) {
        throw std::runtime_error{"failed to initialize freetype2"};
    }

    auto font_path = get_default_font_path();
    error = FT_New_Face(g_library, font_path.c_str(), 0, &g_face);
    if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error{"freetype2 dones not support default font format: " + font_path};
    } else if (error) {
        throw std::runtime_error{"failed to load font file"};
    }
    FT_Set_Pixel_Sizes(g_face, 0, g_font_size);

    // calc text texture size
    int w = 0;
    int h = 0;
    for (int i=32; i<128; i++) {
        if (FT_Load_Char(g_face, i, FT_LOAD_RENDER)) {
            throw std::runtime_error{"failed to load charactor: " + std::string{(char)i}};
        }
        auto const& bitmap = g_face->glyph->bitmap;
        w += bitmap.width;
        h = std::max(h, (int)bitmap.rows);
    }
    g_atlas_width = w;
    g_atlas_height = h;

    // create empty texture with width x height
    glGenTextures(1, &g_texture_id);
    glBindTexture(GL_TEXTURE_2D, g_texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // write font glyph bitmap to texture
    int x = 0;
    for (int i=32; i<128; i++) {
        if (FT_Load_Char(g_face, i, FT_LOAD_RENDER))
            continue;
        auto glyph = g_face->glyph;
        int glyph_w = glyph->bitmap.width;
        int glyph_h = glyph->bitmap.rows;
        std::vector<GLubyte> buf(glyph_w * glyph_h * 4);
        for (int i=0; i<glyph_h * glyph_w; i++) {
            buf[i*4+0] = glyph->bitmap.buffer[i];
            buf[i*4+1] = glyph->bitmap.buffer[i];
            buf[i*4+2] = glyph->bitmap.buffer[i];
            buf[i*4+3] = glyph->bitmap.buffer[i];
        }
        glTextureSubImage2D(g_texture_id, 0, x, 0, glyph->bitmap.width, glyph->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());

        g_char_infos[i].advance_x = glyph->advance.x >> 6;
        g_char_infos[i].width = glyph->bitmap.width;
        g_char_infos[i].height = glyph->bitmap.rows;
        g_char_infos[i].bearing_x = glyph->metrics.horiBearingX >> 6;
        g_char_infos[i].bearing_y = glyph->metrics.horiBearingY >> 6;
        g_char_infos[i].texture_x = x;
        g_char_infos[i].texture_y = 0;

        x += glyph->bitmap.width;
    }
}

void font_manager_t::finalize() {
    glDeleteTextures(1, &g_texture_id);
    glDeleteProgram(g_shader_id);
}

font_manager_t::char_info_t const& font_manager_t::char_info(char c) {
    return g_char_infos[c];
}

GLuint font_manager_t::shader_id() {
    return g_shader_id;
}

GLuint font_manager_t::texture_id() {
    return g_texture_id;
}

GLuint font_manager_t::texture_sampler_id() {
    g_texture_sampler_id = glGetUniformLocation(g_shader_id, "myTextureSampler");
    return g_texture_sampler_id;
}

int font_manager_t::atlas_width() {
    return g_atlas_width;
}

int font_manager_t::atlas_height() {
    return g_atlas_height;
}
