#ifndef FIGHT_ROOM_HPP
#define FIGHT_ROOM_HPP

#include "Component/Map/BaseRoom.hpp"

class FightRoom : public BaseRoom {
public:
    FightRoom(
        const glm::vec2 &absolutePosition,
        const DoorConfig &doorConfig,
        RoomType roomType,
        float wallThickness
    );

    void OnPlayerEnter() override;
    void OnPlayerLeave() override;

private:
    bool m_HasTriggeredLockdown = false;
};

#endif
