#ifndef PROTOWORK_WORLD_TEXT3D_HPP
#define PROTOWORK_WORLD_TEXT3D_HPP

#include <string>
#include <protowork/util.hpp>

struct GLFWwindow;

namespace protowork::world {

struct text3d_t {
    void draw(GLFWwindow *, glm::mat4 const &) const;
    pos_t pos;
    int font_size;
    std::string text;
};

} // namespace protowork::world

#endif
