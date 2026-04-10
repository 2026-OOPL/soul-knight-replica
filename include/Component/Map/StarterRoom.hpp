#ifndef STARTER_ROOM_HPP
#define STARTER_ROOM_HPP

#include "Component/Map/BaseRoom.hpp"

class StarterRoom : public BaseRoom {
public:
    StarterRoom(
        const glm::vec2 &absolutePosition,
        const DoorConfig &doorConfig,
        RoomType roomType,
        float wallThickness
    );

private:
    static DoorConfig BuildStarterDoorConfig(const DoorConfig &doorConfig);
    static WallConfig BuildStarterWallConfig(
        const DoorConfig &doorConfig,
        RoomType roomType,
        float wallThickness
    );
};

#endif
