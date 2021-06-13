#ifndef PROTOWORK_WORLD_CAMERA_HPP
#define PROTOWORK_WORLD_CAMERA_HPP

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
    pos_t m_origin_pos = pos_t{0, 0, 5};
    pos_t m_target_pos = pos_t{0, 0, 0};
};

}; // namespace protowork::world

#endif
