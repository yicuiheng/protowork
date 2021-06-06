#include <cassert>
#include <iostream>
#include <protowork.hpp>

namespace pw = protowork;

struct box_object_t : public pw::model_t {
    explicit box_object_t() {
        m_vertices.push_back(pw::pos_t{-0.5f, 0.5f, 0.5f});
        m_vertices.push_back(pw::pos_t{0.5f, 0.5f, 0.5f});
        m_vertices.push_back(pw::pos_t{0.5f, 0.5f, -0.5f});
        m_vertices.push_back(pw::pos_t{-0.5f, 0.5f, -0.5f});
        m_vertices.push_back(pw::pos_t{-0.5f, -0.5f, 0.5f});
        m_vertices.push_back(pw::pos_t{0.5f, -0.5f, 0.5f});
        m_vertices.push_back(pw::pos_t{0.5f, -0.5f, -0.5f});
        m_vertices.push_back(pw::pos_t{-0.5f, -0.5f, -0.5f});

        m_indices.push_back(0);
        m_indices.push_back(1);
        m_indices.push_back(3);
        m_indices.push_back(2);
        m_indices.push_back(3);
        m_indices.push_back(1);

        m_indices.push_back(0);
        m_indices.push_back(4);
        m_indices.push_back(1);
        m_indices.push_back(5);
        m_indices.push_back(1);
        m_indices.push_back(4);

        m_indices.push_back(1);
        m_indices.push_back(5);
        m_indices.push_back(2);
        m_indices.push_back(6);
        m_indices.push_back(2);
        m_indices.push_back(5);

        m_indices.push_back(2);
        m_indices.push_back(6);
        m_indices.push_back(3);
        m_indices.push_back(7);
        m_indices.push_back(3);
        m_indices.push_back(6);

        m_indices.push_back(3);
        m_indices.push_back(7);
        m_indices.push_back(0);
        m_indices.push_back(4);
        m_indices.push_back(9);
        m_indices.push_back(7);

        m_indices.push_back(7);
        m_indices.push_back(6);
        m_indices.push_back(4);
        m_indices.push_back(5);
        m_indices.push_back(4);
        m_indices.push_back(6);
    }

    std::vector<pw::pos_t> const &vbo_vertex_buffer() const override {
        return m_vertices;
    }
    std::vector<pw::index_t> const &vbo_index_buffer() const override {
        return m_indices;
    }

private:
    std::vector<pw::pos_t> m_vertices;
    std::vector<pw::index_t> m_indices;
};

int main() {
    auto window =
        pw::window_t{pw::window_t::config_t{800, 600, "all test window"}};

    auto box = std::make_shared<box_object_t>();
    window.add_model(box);

    auto text_neko = std::make_shared<pw::text2d_t>(400, 300, 32, "neko");
    window.add_text_2d(text_neko);

    auto text_inu = std::make_shared<pw::text2d_t>(0, 0, 96, "inu");
    window.add_text_2d(text_inu);

    auto text_hoge = std::make_shared<pw::text3d_t>(
        glm::vec3{-0.5f, 0.5f, 0.5f}, 24, "hoge");
    window.add_text_3d(text_hoge);

    while (!window.should_close()) {
        text_inu->x() += 1;
        text_hoge->pos() = box->vbo_vertex_buffer()[0];
        window.update();
        window.draw();
    }
}
