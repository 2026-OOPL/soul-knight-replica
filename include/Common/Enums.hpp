#ifndef TYPES_HPP
#define TYPES_HPP

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

#endif