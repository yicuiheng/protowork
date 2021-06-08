#include <cassert>
#include <iostream>
#include <protowork.hpp>

namespace pw = protowork;

template <typename T, typename U> T as(U x) { return static_cast<T>(x); }

struct sphere_object_t : public pw::model_t {
    explicit sphere_object_t() {
        pw::pos_t top{0.f, 1.f, 0.f};
        constexpr int N_DIVISION = 12;
        m_vertices.push_back(top);
        std::vector<std::pair<int, pw::pos_t>> prev_phi(N_DIVISION,
                                                        std::make_pair(0, top));

        for (int i = 0; i < N_DIVISION; i++) {
            int phi =
                2.f * M_PI * as<float>(i) / as<float>(N_DIVISION) + M_PI / 2.f;
            float y = std::sin(phi);
            float x = std::cos(phi);
            float z = 0.f;
            auto prev_theta =
                std::make_pair(as<int>(m_vertices.size()), glm::vec3{x, y, z});
            m_vertices.push_back(prev_theta.second);

            for (int j = 1; j <= N_DIVISION; j++) {
                int theta = 2.f * M_PI * as<float>(j) / as<float>(N_DIVISION);
                float y = std::sin(phi);
                float x = std::cos(phi) * std::cos(theta);
                float z = std::cos(phi) * std::sin(theta);

                pw::pos_t current{x, y, z};
                int current_id = m_vertices.size();
                if (j == N_DIVISION) {
                    current_id -= N_DIVISION;
                } else {
                    m_vertices.emplace_back(current);
                }

                if (i != 0) {
                    m_indices.emplace_back(prev_phi[j - 1].first);
                    m_indices.emplace_back(prev_theta.first);
                    m_indices.emplace_back(prev_phi[j].first);
                }

                m_indices.emplace_back(current_id);
                m_indices.emplace_back(prev_theta.first);
                m_indices.emplace_back(prev_phi[j].first);

                prev_theta = std::make_pair(current_id, current);
            }
            /*
            for (int i = 0; i < N_DIVISION; i++) {
                int id = m_vertices.size() - N_DIVISION + i;
                prev_phi[i] = std::make_pair(id, m_vertices[id]);
            } */
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

    auto box = std::make_shared<sphere_object_t>();
    window.add_model(box);

    auto text_neko = std::make_shared<pw::text2d_t>(400, 300, 32, "neko");
    window.add_text_2d(text_neko);

    auto text_inu = std::make_shared<pw::text2d_t>(0, 0, 96, "inu");
    window.add_text_2d(text_inu);

    auto const &vertices = box->vbo_vertex_buffer();
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
