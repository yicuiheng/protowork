#ifndef PROTOWORK_UI_HPP
#define PROTOWORK_UI_HPP

#include <vector>
#include <memory>
#include <protowork/ui/text2d.hpp>

namespace protowork {

struct ui_t {
    std::vector<std::shared_ptr<ui::text2d_t>> texts_2d;
};

} // namespace protowork

#endif
