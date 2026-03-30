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

struct DoorSideConfig {
    bool hasDoor = false;
    bool startsOpen = true;
};

struct DoorConfig {
    DoorSideConfig top;
    DoorSideConfig right;
    DoorSideConfig bottom;
    DoorSideConfig left;
};

struct WallSideConfig {
    bool hasOpening = false;
    float thickness = 20.0F;
    float openingOffset = 0.0F;
    float openingSize = 0.0F;
};

struct WallConfig {
    WallSideConfig top;
    WallSideConfig right;
    WallSideConfig bottom;
    WallSideConfig left;
};

#endif
