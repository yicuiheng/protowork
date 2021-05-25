#ifndef PROTOWORK_HPP
#define PROTOWORK_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <numbers>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace protowork {

using matrix_t = glm::mat4;
using pos_t = glm::vec3;
using index_t = GLushort;
using id_t = GLuint;

namespace detail {
    id_t load_shader_program();

    struct camera_t {
        explicit camera_t();
        void update(GLFWwindow*);
        matrix_t projection() const;
        matrix_t view() const;
    private:
        matrix_t m_view;
        pos_t m_pos = pos_t{0, 0, 5};
        float m_horizontal_angle = std::numbers::pi_v<float>;
        float m_vertical_angle = 0.f;
        double m_last_time;
    };
}

struct model_t {
    explicit model_t();
    virtual ~model_t();

    void draw() const;
protected:
    virtual std::vector<pos_t> const& vbo_vertex_buffer() const = 0;
    virtual std::vector<index_t> const& vbo_index_buffer() const = 0;
private:
    id_t m_vertex_array_id;
    id_t m_vbo_vertex_buffer_id;
    id_t m_vbo_index_buffer_id;
};

struct window_t {
    struct config_t {
        std::size_t width;
        std::size_t height;
        const char* title;
    };
    explicit window_t(config_t const&);
    ~window_t();

    void add_model(std::shared_ptr<model_t> const& model) {
        m_models.push_back(model);
    }

    void update();
    void draw() const;

    bool should_close() const;
private:
    GLFWwindow* m_window = nullptr;
    std::vector<std::shared_ptr<model_t>> m_models;

    id_t m_shader_id;
    id_t m_mvp_matrix_id;
    id_t m_view_matrix_id;
    id_t m_model_matrix_id;
    id_t m_light_id;
    detail::camera_t m_camera;
};

}

#endif
