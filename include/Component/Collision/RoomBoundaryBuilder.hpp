#ifndef ROOM_BOUNDARY_BUILDER_HPP
#define ROOM_BOUNDARY_BUILDER_HPP

#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionTypes.hpp"

namespace Collision {

struct WallOpening {
    float size = 0.0F;
    float offset = 0.0F;
};

struct RoomBoundaryOpenings {
    WallOpening top;
    WallOpening right;
    WallOpening bottom;
    WallOpening left;
};

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness
);

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness,
    const RoomBoundaryOpenings &openings
);

} // namespace Collision

#endif
