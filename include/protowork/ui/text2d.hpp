#ifndef PROTOWORK_UI_TEXT2D_HPP
#define PROTOWORK_UI_TEXT2D_HPP

#include <string>

namespace protowork::ui {

struct text2d_t {
    int x, y, font_size;
    std::string text;

    void append(std::vector<glm::vec2> &vertices,
                std::vector<glm::vec2> &uvs) const;
};

} // namespace protowork::ui

#endif
