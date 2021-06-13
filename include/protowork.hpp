#ifndef PROTOWORK_HPP
#define PROTOWORK_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <protowork/input.hpp>
#include <protowork/world.hpp>
#include <protowork/ui.hpp>

struct GLFWwindow;

namespace protowork {

struct app_t {
    struct config_t {
        std::size_t width;
        std::size_t height;
        const char *title;
    };
    explicit app_t(config_t const &);
    explicit app_t(std::size_t width, std::size_t height, const char *title)
        : app_t{config_t{width, height, title}} {}

    ~app_t();

    void update();
    void draw() const;

    bool should_close() const;

    world_t world;
    ui_t ui;

private:
    GLFWwindow *m_window = nullptr;
    input_t m_input;
};

} // namespace protowork

#endif
