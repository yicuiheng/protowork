#ifndef PROTOWORK_UI_TEXT2D_HPP
#define PROTOWORK_UI_TEXT2D_HPP

#include <string>

struct GLFWwindow;

namespace protowork::ui {

struct text2d_t {
    int x, y, font_size;
    std::string text;

    void draw(GLFWwindow *) const;
};

} // namespace protowork::ui

#endif
