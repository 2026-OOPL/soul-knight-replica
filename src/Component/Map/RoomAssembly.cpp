#include "Component/Map/RoomAssembly.hpp"

#include <array>
#include <stdexcept>
#include <utility>

#include "Util/Image.hpp"
#include "Util/Logger.hpp"

namespace {

constexpr char kHorizontalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_5x2.png";
constexpr char kVerticalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_1x5.png";
const glm::vec2 kHorizontalDoorColliderSize = {80.0F, 15.0F};
const glm::vec2 kHorizontalDoorRenderSize = {80.0F, 32.0F};
const glm::vec2 kVerticalDoorColliderSize = {15.0F, 80.0F};
const glm::vec2 kVerticalDoorRenderSize = {16.0F, 96.0F};
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

std::size_t SideToIndex(DoorSide side) {
    switch (side) {
    case DoorSide::Top:
        return 0;
    case DoorSide::Right:
        return 1;
    case DoorSide::Bottom:
        return 2;
    case DoorSide::Left:
        return 3;
    }

    return 0;
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

void ApplyDoorOpening(
    Collision::RoomBoundaryOpenings &wallOpenings,
    std::array<bool, 4> &occupiedSides,
    const DoorConfig &doorConfig
) {
    const std::size_t sideIndex = SideToIndex(doorConfig.side);
    if (occupiedSides[sideIndex]) {
        throw std::invalid_argument("RoomAssembly supports at most one door per side.");
    }
    occupiedSides[sideIndex] = true;

    switch (doorConfig.side) {
    case DoorSide::Top:
        wallOpenings.top.size = doorConfig.colliderSize.x;
        wallOpenings.top.offset = doorConfig.openingOffset;
        break;

    case DoorSide::Right:
        wallOpenings.right.size = doorConfig.colliderSize.y;
        wallOpenings.right.offset = doorConfig.openingOffset;
        break;

    case DoorSide::Bottom:
        wallOpenings.bottom.size = doorConfig.colliderSize.x;
        wallOpenings.bottom.offset = doorConfig.openingOffset;
        break;

    case DoorSide::Left:
        wallOpenings.left.size = doorConfig.colliderSize.y;
        wallOpenings.left.offset = doorConfig.openingOffset;
        break;
    }
}

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

std::vector<DoorConfig> BuildDefaultDoorConfigs() {
    DoorConfig bottomDoorConfig;
    bottomDoorConfig.side = DoorSide::Bottom;
    bottomDoorConfig.colliderSize = kHorizontalDoorColliderSize;
    bottomDoorConfig.renderSize = kHorizontalDoorRenderSize;
    bottomDoorConfig.visuals = BuildHorizontalDoorVisuals();
    bottomDoorConfig.positionOffset = {0.0F, kBottomDoorVisualOffsetY};

    DoorConfig topDoorConfig;
    topDoorConfig.side = DoorSide::Top;
    topDoorConfig.colliderSize = kHorizontalDoorColliderSize;
    topDoorConfig.renderSize = kHorizontalDoorRenderSize;
    topDoorConfig.visuals = BuildHorizontalDoorVisuals();
    topDoorConfig.positionOffset = {0.0F, kTopDoorVisualOffsetY};

    DoorConfig leftDoorConfig;
    leftDoorConfig.side = DoorSide::Left;
    leftDoorConfig.colliderSize = kVerticalDoorColliderSize;
    leftDoorConfig.renderSize = kVerticalDoorRenderSize;
    leftDoorConfig.visuals = BuildVerticalDoorVisuals();

    DoorConfig rightDoorConfig;
    rightDoorConfig.side = DoorSide::Right;
    rightDoorConfig.colliderSize = kVerticalDoorColliderSize;
    rightDoorConfig.renderSize = kVerticalDoorRenderSize;
    rightDoorConfig.visuals = BuildVerticalDoorVisuals();

    return {
        bottomDoorConfig,
        topDoorConfig,
        leftDoorConfig,
        rightDoorConfig
    };
}

} // namespace

RoomAssembly::RoomAssembly(RoomAssemblyConfig config) {
    this->m_Room = std::make_shared<BaseRoom>(config);
    this->m_Pieces.push_back(this->m_Room);

    const glm::vec2 roomCenter = this->m_Room->GetAbsoluteCooridinate();
    const glm::vec2 roomSize = this->m_Room->GetAbsoluteScale();
    const std::vector<DoorConfig> doorConfigs = BuildDefaultDoorConfigs();
    Collision::RoomBoundaryOpenings wallOpenings;
    std::array<bool, 4> occupiedSides = {false, false, false, false};

    for (const DoorConfig &doorConfig : doorConfigs) {
        ApplyDoorOpening(wallOpenings, occupiedSides, doorConfig);
    }

    this->m_StaticWallBoxes = Collision::BuildRoomBoundaryBoxes(
        roomCenter,
        roomSize,
        config.wallThickness,
        wallOpenings
    );

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

const std::vector<Collision::AxisAlignedBox> &RoomAssembly::GetStaticWallBoxes() const {
    return this->m_StaticWallBoxes;
}

glm::vec2 RoomAssembly::GetSuggestedBottomSpawn(float distanceFromRoom) const {
    if (this->m_Room == nullptr) {
        return {0.0F, 0.0F};
    }

    const glm::vec2 roomCenter = this->m_Room->GetAbsoluteCooridinate();
    const glm::vec2 roomSize = this->m_Room->GetAbsoluteScale();

    return {
        roomCenter.x,
        roomCenter.y - roomSize.y / 2.0F - distanceFromRoom
    };
}

void RoomAssembly::CloseAllDoors() {
    for (const auto &door : this->m_Doors) {
        if (door != nullptr) {
            door->Close();
        }
    }
}
