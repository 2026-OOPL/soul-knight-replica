#include "Component/Map/StarterRoom.hpp"

StarterRoom::StarterRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    RoomType roomType,
    float wallThickness
)
    : BaseRoom(
          absolutePosition,
          roomType,
          RoomPurpose::STARTER,
          BuildStarterDoorConfig(doorConfig),
          BuildStarterWallConfig(doorConfig, roomType, wallThickness)
      ) {
}

DoorConfig StarterRoom::BuildStarterDoorConfig(const DoorConfig &doorConfig) {
    DoorConfig starterDoorConfig = doorConfig;
    starterDoorConfig.top.hasDoor = false;
    starterDoorConfig.right.hasDoor = false;
    starterDoorConfig.bottom.hasDoor = false;
    starterDoorConfig.left.hasDoor = false;
    return starterDoorConfig;
}

WallConfig StarterRoom::BuildStarterWallConfig(
    const DoorConfig &doorConfig,
    RoomType roomType,
    float wallThickness
) {
    WallConfig wallConfig = BaseRoom::BuildWallConfigFromDoorConfig(
        doorConfig,
        wallThickness,
        roomType,
        RoomPurpose::STARTER
    );
    wallConfig.top.hasOpening = doorConfig.top.hasDoor;
    wallConfig.right.hasOpening = doorConfig.right.hasDoor;
    wallConfig.bottom.hasOpening = doorConfig.bottom.hasDoor;
    wallConfig.left.hasOpening = doorConfig.left.hasDoor;
    return wallConfig;
}
