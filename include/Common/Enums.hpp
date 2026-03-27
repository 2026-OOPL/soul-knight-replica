#ifndef TYPES_HPP
#define TYPES_HPP

#include <glm/vec2.hpp>

enum class Direction {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
};

enum class RoomType {
    ROOM_13_13,
    ROOM_15_15,
    ROOM_17_17,
    ROOM_17_23,
    ROOM_23_17
};

enum class RoomPurpose {
    STARTER,
    FIGHTING,
    REWARD,
    PORTAL
};

struct RoomAssemblyConfig {
    RoomType type = RoomType::ROOM_13_13;
    glm::vec2 roomCenter = {0.0F, 0.0F};
    float wallThickness = 20.0F;
};

#endif