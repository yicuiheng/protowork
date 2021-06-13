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

protected:
    virtual std::vector<pos_t> const &vbo_vertex_buffer() const = 0;
    virtual std::vector<glm::vec3> const &vbo_normal_buffer() const = 0;
    virtual std::vector<index_t> const &vbo_index_buffer() const = 0;

private:
    id_t m_vertex_array_id;
    id_t m_vbo_vertex_buffer_id;
    id_t m_vbo_normal_buffer_id;
    id_t m_vbo_index_buffer_id;
};

} // namespace protowork::world

#endif
