#include "Component/Map/FightRoom.hpp"

FightRoom::FightRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    RoomType roomType,
    float wallThickness
)
    : BaseRoom(
          absolutePosition,
          roomType,
          RoomPurpose::FIGHTING,
          doorConfig,
          BaseRoom::BuildWallConfigFromDoorConfig(doorConfig, wallThickness)
      ) {
    this->OpenAllDoors();
}

void FightRoom::OnPlayerEnter() {
    if (this->m_HasTriggeredLockdown) {
        return;
    }

    this->CloseAllDoors();
    this->m_HasTriggeredLockdown = true;
}

void FightRoom::OnPlayerLeave() {
}
