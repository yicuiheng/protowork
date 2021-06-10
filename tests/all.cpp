#include <cassert>
#include <iostream>
#include <cmath>
#include <protowork.hpp>

namespace pw = protowork;

pw::pos_t angle_to_pos(float phi, float theta) {
    float sin_phi = std::sin(phi);
    float cos_phi = std::cos(phi);
    float sin_theta = std::sin(theta);
    float cos_theta = std::cos(theta);
    return pw::pos_t{sin_phi * cos_theta, cos_phi, sin_phi * sin_theta};
}

struct sphere_object_t : public pw::model_t {
    explicit sphere_object_t() {
        pw::pos_t top{0.f, 1.f, 0.f};
        constexpr int N_DIVISION = 8;
        m_vertices.push_back(top);
        using row_t = std::vector<std::pair<int, pw::pos_t>>;
        row_t prev_row(N_DIVISION + 1, std::make_pair(0, top));

        for (int i = 1; i <= N_DIVISION; i++) {
            float phi =
                M_PI * static_cast<float>(i) / static_cast<float>(N_DIVISION);
            auto init_pos = angle_to_pos(phi, 0.f);
            auto prev_theta =
                std::make_pair(static_cast<int>(m_vertices.size()), init_pos);
            m_vertices.push_back(prev_theta.second);

            row_t current_row;
            current_row.push_back(prev_theta);

            for (int j = 1; j <= N_DIVISION; j++) {
                float theta = 2.f * M_PI * static_cast<float>(j) /
                              static_cast<float>(N_DIVISION);
                auto current = angle_to_pos(phi, theta);

                int current_id = m_vertices.size();

                if (j == N_DIVISION) {
                    current_id -= N_DIVISION;
                } else {
                    m_vertices.emplace_back(current);
                }

                if (i != 1) {
                    m_indices.emplace_back(prev_row[j - 1].first);
                    m_indices.emplace_back(prev_row[j].first);
                    m_indices.emplace_back(prev_theta.first);
                }
                if (i != N_DIVISION) {
                    m_indices.emplace_back(current_id);
                    m_indices.emplace_back(prev_theta.first);
                    m_indices.emplace_back(prev_row[j].first);
                }

                prev_theta = std::make_pair(current_id, current);
                current_row.push_back(prev_theta);
            }
            prev_row = std::move(current_row);
        }
    }

    std::vector<pw::pos_t> const &vbo_vertex_buffer() const override {
        return m_vertices;
    }
    std::vector<glm::vec3> const &vbo_normal_buffer() const override {
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

    auto sphere = std::make_shared<sphere_object_t>();
    window.add_model(sphere);

    auto text_neko = std::make_shared<pw::text2d_t>(400, 300, 32, "neko");
    window.add_text_2d(text_neko);

    auto text_inu = std::make_shared<pw::text2d_t>(0, 0, 96, "inu");
    window.add_text_2d(text_inu);

    auto const &vertices = sphere->vbo_vertex_buffer();
    std::vector<std::shared_ptr<pw::text3d_t>> text_vertices;
    for (int i = 0; i < vertices.size(); i++) {
        auto t = std::make_shared<pw::text3d_t>(vertices[i], 24,
                                                "(" + std::to_string(i) + ")");
        text_vertices.push_back(t);
        window.add_text_3d(t);
    }

    while (!window.should_close()) {
        text_inu->x += 1;
        for (int i = 0; i < vertices.size(); i++) {
            text_vertices[i]->pos = vertices[i];
        }
        window.update();
        window.draw();
    }
}
