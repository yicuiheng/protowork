#ifndef PROTOWORK_HPP
#define PROTOWORK_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace protowork {

using matrix_t = glm::mat4;
using pos_t = glm::vec3;
using index_t = GLushort;
using id_t = GLuint;

namespace detail {
id_t load_shader_program(const char *, const char *);
}

struct input_t {
    struct mouse_t {
        enum button_t { LEFT = 0, RIGHT, MIDDLE, N_BUTTONS };
        enum class button_state_t { RELEASED, PRESSED, PUSHED };
        double x = 0.0;
        double y = 0.0;
        button_state_t buttons[button_t::N_BUTTONS] = {};
    } mouse;
};

struct camera_t {
    explicit camera_t();
    void update(input_t const &);
    matrix_t projection() const;
    matrix_t view() const;

private:
    void
    rotate_around_target(input_t::mouse_t const &); // drag with left button
    matrix_t m_view;
    pos_t m_origin_pos = pos_t{0, 0, 5};
    pos_t m_target_pos = pos_t{0, 0, 0};
};

struct model_t {
    explicit model_t();
    virtual ~model_t();

    void draw() const;

protected:
    virtual std::vector<pos_t> const &vbo_vertex_buffer() const = 0;
    virtual std::vector<index_t> const &vbo_index_buffer() const = 0;

private:
    id_t m_vertex_array_id;
    id_t m_vbo_vertex_buffer_id;
    id_t m_vbo_index_buffer_id;
};

namespace font {

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
    GLuint texture_id;
    int atlas_width;
    int atlas_height;
};

void initialize();
void finalize();

data_t const &get(key_t const &);

GLuint shader_id();
GLuint texture_sampler_id();

} // namespace font

struct text2d_t {
    explicit text2d_t(int x, int y, int font_size, std::string const &str);
    void draw() const;
    virtual ~text2d_t();

private:
    int m_x, m_y;
    int m_font_size;
    std::string m_text;
    std::vector<glm::vec2> m_vertices;
    std::vector<glm::vec2> m_uvs;
    GLuint m_vertex_buffer_id;
    GLuint m_uv_buffer_id;
};

struct window_t {
    struct config_t {
        std::size_t width;
        std::size_t height;
        const char *title;
    };
    explicit window_t(config_t const &);
    ~window_t();

    void add_model(std::shared_ptr<model_t> const &model) {
        m_models.push_back(model);
    }
    void add_text_2d(std::shared_ptr<text2d_t> const &text) {
        m_text_2ds.push_back(text);
    }

    void update();
    void draw() const;

    bool should_close() const;

private:
    GLFWwindow *m_window = nullptr;
    std::vector<std::shared_ptr<model_t>> m_models;
    std::vector<std::shared_ptr<text2d_t>> m_text_2ds;

    id_t m_shader_id;
    id_t m_mvp_matrix_id;
    id_t m_view_matrix_id;
    id_t m_model_matrix_id;
    id_t m_light_id;
    camera_t m_camera;

    input_t m_input;
};

} // namespace protowork

#endif
