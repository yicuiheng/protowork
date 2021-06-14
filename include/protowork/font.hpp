#ifndef PROTOWORK_FONT_HPP
#define PROTOWORK_FONT_HPP

#include <unordered_map>
#include <protowork/util.hpp>

struct GLFWwindow;

namespace protowork::font {

struct key_t {
    int font_size;
};

bool operator==(key_t const &, key_t const &);

struct char_info_t {
    int advance_x;
    int width;
    int height;
    int bearing_x;
    int bearing_y;
    int texture_x;
    int texture_y;
};

struct data_t {
    std::unordered_map<int, char_info_t> char_infos;
    id_t texture_id;
    int atlas_width;
    int atlas_height;
};

void initialize();
void finalize();
void before_drawing();

void render(GLFWwindow *window, int font_size,
            std::vector<glm::vec2> const &vertices,
            std::vector<glm::vec2> const &uvs);

data_t const &get(key_t const &);

} // namespace protowork::font

#endif
