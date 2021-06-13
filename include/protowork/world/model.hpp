#ifndef PROTOWORK_WORLD_MODEL_HPP
#define PROTOWORK_WORLD_MODEL_HPP

#include <vector>
#include <protowork/world/camera.hpp>

namespace protowork::world {

struct model_t {
    explicit model_t();
    virtual ~model_t();

    void draw() const;

    static void initialize(); // initialize shader for model_t
    static void finalize();   // finalize for model_t
    static void before_drawing(camera_t const &);

    std::vector<pos_t> vertices;
    std::vector<glm::vec3> normals;
    std::vector<index_t> indices;
    matrix_t model_matrix = matrix_t(1.f);

private:
    id_t m_vertex_array_id;
    id_t m_vertex_buffer_id;
    id_t m_normal_buffer_id;
    id_t m_index_buffer_id;
};

} // namespace protowork::world

#endif
