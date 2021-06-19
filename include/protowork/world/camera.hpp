#ifndef PROTOWORK_WORLD_CAMERA_HPP
#define PROTOWORK_WORLD_CAMERA_HPP

#include <glm/gtx/quaternion.hpp>

#include <protowork/util.hpp>
#include <protowork/input.hpp>

namespace protowork::world {

struct camera_t {
    explicit camera_t();
    void update(protowork::input_t const &);
    matrix_t projection() const;
    matrix_t view() const;

private:
    matrix_t m_view;
    glm::quat m_orientation;
    float m_distance = 5.f;
    pos_t m_target_pos = pos_t{0, 0, 0};
};

}; // namespace protowork::world

#endif
