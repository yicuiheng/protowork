#ifndef PROTOWORK_WORLD_TEXT3D_HPP
#define PROTOWORK_WORLD_TEXT3D_HPP

#include <string>
#include <protowork/util.hpp>

struct GLFWwindow;

namespace protowork::world {

struct text3d_t {
    void append(GLFWwindow *window, glm::mat4 const &,
                std::vector<glm::vec2> &vertices,
                std::vector<glm::vec2> &uvs) const;
    pos_t pos;
    int font_size;
    std::string text;
};

} // namespace protowork::world

#endif
