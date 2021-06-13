#ifndef PROTOWORK_WORLD_HPP
#define PROTOWORK_WORLD_HPP

#include <vector>
#include <memory>

#include <protowork/world/camera.hpp>
#include <protowork/world/model.hpp>
#include <protowork/world/text3d.hpp>

namespace protowork {

struct world_t {
    std::vector<std::shared_ptr<world::model_t>> models;
    std::vector<std::shared_ptr<world::text3d_t>> texts_3d;
    world::camera_t camera;
};

} // namespace protowork

#endif
