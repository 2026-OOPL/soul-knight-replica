#include "Component/Map/RoomAssembly.hpp"

#include <utility>

#include "Component/Map/MapColliderConfig.hpp"
#include "Component/Map/Room13x13.hpp"
#include "Component/Map/Room15x15.hpp"
#include "Component/Map/Room17x17.hpp"
#include "Component/Map/Room17x23.hpp"
#include "Component/Map/Room23x17.hpp"
#include "Util/Image.hpp"

namespace {

constexpr char kHorizontalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_5x2.png";
constexpr char kVerticalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_1x5.png";
constexpr float kBottomDoorVisualOffsetY = 15.0F;
constexpr float kTopDoorVisualOffsetY = 10.0F;

struct DoorConfig {
    DoorSide side = DoorSide::Bottom;
    glm::vec2 colliderSize = {0.0F, 0.0F};
    glm::vec2 renderSize = {0.0F, 0.0F};
    Door::Visuals visuals;
    float openingOffset = 0.0F;
    glm::vec2 positionOffset = {0.0F, 0.0F};
    bool isOpen = true;
};

Door::Visuals BuildHorizontalDoorVisuals() {
    Door::Visuals visuals;
    visuals.closedIdle = std::make_shared<Util::Image>(kHorizontalClosedDoorSprite);
    return visuals;
}

Door::Visuals BuildVerticalDoorVisuals() {
    Door::Visuals visuals;
    visuals.closedIdle = std::make_shared<Util::Image>(kVerticalClosedDoorSprite);
    return visuals;
}

std::shared_ptr<BaseRoom> BuildRoom(RoomAssemblyConfig config) {
    switch (config.type) {
    case RoomType::ROOM_13_13:
        return std::make_shared<Room13x13>(std::move(config));

    case RoomType::ROOM_15_15:
        return std::make_shared<Room15x15>(std::move(config));

    case RoomType::ROOM_17_17:
        return std::make_shared<Room17x17>(std::move(config));

    case RoomType::ROOM_17_23:
        return std::make_shared<Room17x23>(std::move(config));

    case RoomType::ROOM_23_17:
        return std::make_shared<Room23x17>(std::move(config));
    }

    return std::make_shared<Room13x13>(std::move(config));
}

glm::vec2 BuildDoorPosition(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    const DoorConfig &doorConfig
) {
    switch (doorConfig.side) {
    case DoorSide::Top:
        return {
            roomCenter.x + doorConfig.openingOffset,
            roomCenter.y + roomSize.y / 2.0F - doorConfig.renderSize.y / 2.0F
        };

    case DoorSide::Right:
        return {
            roomCenter.x + roomSize.x / 2.0F - doorConfig.renderSize.x / 2.0F,
            roomCenter.y + doorConfig.openingOffset
        };

    case DoorSide::Bottom:
        return {
            roomCenter.x + doorConfig.openingOffset,
            roomCenter.y - roomSize.y / 2.0F + doorConfig.renderSize.y / 2.0F
        };

    case DoorSide::Left:
        return {
            roomCenter.x - roomSize.x / 2.0F + doorConfig.renderSize.x / 2.0F,
            roomCenter.y + doorConfig.openingOffset
        };
    }

    return roomCenter;
}

void AppendDoorConfig(
    std::vector<DoorConfig> &doorConfigs,
    const WallDoorConfig &wallConfig,
    DoorSide side
) {
    if (!wallConfig.hasDoor) {
        return;
    }

    DoorConfig doorConfig;
    doorConfig.side = side;
    doorConfig.openingOffset = wallConfig.openingOffset;
    doorConfig.isOpen = wallConfig.startsOpen;

    switch (side) {
    case DoorSide::Top:
        doorConfig.colliderSize = {
            wallConfig.openingSize > 0.0F ?
            wallConfig.openingSize :
            MapColliderConfig::kHorizontalDoorColliderSize.x,
            MapColliderConfig::kHorizontalDoorColliderSize.y
        };
        doorConfig.renderSize = MapColliderConfig::kHorizontalDoorRenderSize;
        doorConfig.visuals = BuildHorizontalDoorVisuals();
        doorConfig.positionOffset = {0.0F, kTopDoorVisualOffsetY};
        break;

    case DoorSide::Right:
        doorConfig.colliderSize = {
            MapColliderConfig::kVerticalDoorColliderSize.x,
            wallConfig.openingSize > 0.0F ?
            wallConfig.openingSize :
            MapColliderConfig::kVerticalDoorColliderSize.y
        };
        doorConfig.renderSize = MapColliderConfig::kVerticalDoorRenderSize;
        doorConfig.visuals = BuildVerticalDoorVisuals();
        break;

    case DoorSide::Bottom:
        doorConfig.colliderSize = {
            wallConfig.openingSize > 0.0F ?
            wallConfig.openingSize :
            MapColliderConfig::kHorizontalDoorColliderSize.x,
            MapColliderConfig::kHorizontalDoorColliderSize.y
        };
        doorConfig.renderSize = MapColliderConfig::kHorizontalDoorRenderSize;
        doorConfig.visuals = BuildHorizontalDoorVisuals();
        doorConfig.positionOffset = {0.0F, kBottomDoorVisualOffsetY};
        break;

    case DoorSide::Left:
        doorConfig.colliderSize = {
            MapColliderConfig::kVerticalDoorColliderSize.x,
            wallConfig.openingSize > 0.0F ?
            wallConfig.openingSize :
            MapColliderConfig::kVerticalDoorColliderSize.y
        };
        doorConfig.renderSize = MapColliderConfig::kVerticalDoorRenderSize;
        doorConfig.visuals = BuildVerticalDoorVisuals();
        break;
    }

    doorConfigs.push_back(doorConfig);
}

std::vector<DoorConfig> BuildDoorConfigs(const RoomBoundaryConfig &boundaries) {
    std::vector<DoorConfig> doorConfigs;
    AppendDoorConfig(doorConfigs, boundaries.top, DoorSide::Top);
    AppendDoorConfig(doorConfigs, boundaries.right, DoorSide::Right);
    AppendDoorConfig(doorConfigs, boundaries.bottom, DoorSide::Bottom);
    AppendDoorConfig(doorConfigs, boundaries.left, DoorSide::Left);
    return doorConfigs;
}

} // namespace

RoomAssembly::RoomAssembly(RoomAssemblyConfig config)
    : m_Config(config) {
    this->m_Room = BuildRoom(config);
    this->m_Pieces.push_back(this->m_Room);

    const glm::vec2 roomCenter = this->m_Room->GetAbsoluteCooridinate();
    const glm::vec2 roomSize = this->m_Room->GetRoomSize();
    const std::vector<DoorConfig> doorConfigs = BuildDoorConfigs(this->m_Config.boundaries);

    for (const DoorConfig &doorConfig : doorConfigs) {
        const glm::vec2 doorPosition =
            BuildDoorPosition(roomCenter, roomSize, doorConfig) + doorConfig.positionOffset;

        const std::shared_ptr<Door> door = std::make_shared<Door>(
            doorPosition,
            doorConfig.side,
            doorConfig.colliderSize,
            doorConfig.renderSize,
            doorConfig.visuals,
            doorConfig.isOpen
        );

        this->m_Doors.push_back(door);
        this->m_Pieces.push_back(door);
    }

    this->m_Room->SetDoors(this->m_Doors);
}

const std::shared_ptr<BaseRoom> &RoomAssembly::GetRoom() const {
    return this->m_Room;
}

const std::vector<std::shared_ptr<Door>> &RoomAssembly::GetDoors() const {
    return this->m_Doors;
}

const std::vector<std::shared_ptr<MapPiece>> &RoomAssembly::GetPieces() const {
    return this->m_Pieces;
}

glm::vec2 RoomAssembly::GetSuggestedBottomSpawn(float distanceFromRoom) const {
    if (this->m_Room == nullptr) {
        return {0.0F, 0.0F};
    }

    const glm::vec2 roomCenter = this->m_Room->GetAbsoluteCooridinate();
    const glm::vec2 roomSize = this->m_Room->GetRoomSize();

    return {
        roomCenter.x,
        roomCenter.y - roomSize.y / 2.0F - distanceFromRoom
    };
}

RoomPurpose RoomAssembly::GetPurpose() const {
    return this->m_Config.purpose;
}

void RoomAssembly::CloseAllDoors() {
    for (const auto &door : this->m_Doors) {
        if (door != nullptr) {
            door->Close();
        }
    }
}
