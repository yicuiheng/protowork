#include <cassert>
#include <iostream>
#include <cmath>
#include <protowork.hpp>
#include <protowork/world.hpp>

namespace pw = protowork;

pw::pos_t angle_to_pos(float phi, float theta) {
    float sin_phi = std::sin(phi);
    float cos_phi = std::cos(phi);
    float sin_theta = std::sin(theta);
    float cos_theta = std::cos(theta);
    return pw::pos_t{sin_phi * cos_theta, cos_phi, sin_phi * sin_theta};
}

struct sphere_object_t : public pw::world::model_t {
    explicit sphere_object_t() {
        pw::pos_t top{0.f, 1.f, 0.f};
        constexpr int N_DIVISION = 8;
        vertices.push_back(top);
        using row_t = std::vector<std::pair<int, pw::pos_t>>;
        row_t prev_row(N_DIVISION + 1, std::make_pair(0, top));

        for (int i = 1; i <= N_DIVISION; i++) {
            float phi =
                M_PI * static_cast<float>(i) / static_cast<float>(N_DIVISION);
            auto init_pos = angle_to_pos(phi, 0.f);
            auto prev_theta =
                std::make_pair(static_cast<int>(vertices.size()), init_pos);
            vertices.push_back(prev_theta.second);

            row_t current_row;
            current_row.push_back(prev_theta);

            for (int j = 1; j <= N_DIVISION; j++) {
                float theta = 2.f * M_PI * static_cast<float>(j) /
                              static_cast<float>(N_DIVISION);
                auto current = angle_to_pos(phi, theta);

                int current_id = vertices.size();

                if (j == N_DIVISION) {
                    current_id -= N_DIVISION;
                } else {
                    vertices.emplace_back(current);
                }

                if (i != 1) {
                    indices.emplace_back(prev_row[j - 1].first);
                    indices.emplace_back(prev_row[j].first);
                    indices.emplace_back(prev_theta.first);
                }
                if (i != N_DIVISION) {
                    indices.emplace_back(current_id);
                    indices.emplace_back(prev_theta.first);
                    indices.emplace_back(prev_row[j].first);
                }

                prev_theta = std::make_pair(current_id, current);
                current_row.push_back(prev_theta);
            }
            prev_row = std::move(current_row);
        }

        normals = vertices;
    }
};

int main() {
    auto app = pw::app_t{800, 600, "all test window"};

    auto sphere = std::make_shared<sphere_object_t>();
    app.world.models.push_back(sphere);

    auto text_neko = std::make_shared<pw::ui::text2d_t>(400, 300, 32, "neko");
    app.ui.texts_2d.push_back(text_neko);

    auto text_inu = std::make_shared<pw::ui::text2d_t>(0, 0, 96, "inu");
    app.ui.texts_2d.push_back(text_inu);

    auto const &vertices = sphere->vertices;
    std::vector<std::shared_ptr<pw::world::text3d_t>> text_vertices;
    for (int i = 0; i < vertices.size(); i++) {
        auto t = std::make_shared<pw::world::text3d_t>(
            vertices[i], 24, "(" + std::to_string(i) + ")");
        text_vertices.push_back(t);
        app.world.texts_3d.push_back(t);
    }

    while (!app.should_close()) {
        text_inu->x += 1;
        for (int i = 0; i < vertices.size(); i++) {
            text_vertices[i]->pos = vertices[i];
        }
        app.update();
        app.draw();
    }
}
