#include "Component/Map/BossRoom.hpp"

#include "Common/Enums.hpp"

BossRoom::BossRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness
) : FightRoom(
        absolutePosition,
        doorConfig,
        info,
        wallThickness,
        RoomPurpose::BOSS
    ) {
}
