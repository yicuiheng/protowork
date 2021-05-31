#include <cassert>
#include <iostream>
#include <protowork.hpp>

namespace pw = protowork;

struct box_object_t : public pw::model_t {
    explicit box_object_t() {
        /*
        m_vertices.push_back(pw::pos_t{-0.5f,  0.5f,  0.5f});
        m_vertices.push_back(pw::pos_t{ 0.5f,  0.5f,  0.5f});
        m_vertices.push_back(pw::pos_t{ 0.5f,  0.5f, -0.5f});
        m_vertices.push_back(pw::pos_t{-0.5f,  0.5f, -0.5f});
        m_vertices.push_back(pw::pos_t{-0.5f, -0.5f,  0.5f});
        m_vertices.push_back(pw::pos_t{ 0.5f, -0.5f,  0.5f});
        m_vertices.push_back(pw::pos_t{ 0.5f, -0.5f, -0.5f});
        m_vertices.push_back(pw::pos_t{-0.5f, -0.5f, -0.5f});
        */
        m_vertices.push_back(pw::pos_t{-1.f, -1.f, 0.f});
        m_vertices.push_back(pw::pos_t{1.f, -1.f, 0.f});
        m_vertices.push_back(pw::pos_t{0.f, 1.f, 0.f});

        /*
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
        */
        m_indices.push_back(0);
        m_indices.push_back(1);
        m_indices.push_back(2);
    }

protected:
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
        pw::window_t{pw::window_t::config_t{640, 480, "all test window"}};

    auto box = std::make_shared<box_object_t>();

    window.add_model(box);

    while (!window.should_close()) {
        window.update();

        pw::print_text_2d(120, 120, "hogehoge");

        window.draw();
    }
}
