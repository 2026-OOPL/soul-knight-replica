#include "Component/Map/BaseRoom.hpp"

#include "Common/Constants.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Map/MapColliderConfig.hpp"
#include "Util/Image.hpp"

namespace {

constexpr char kHorizontalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_5x2.png";
constexpr char kVerticalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_1x5.png";
constexpr float kBottomDoorVisualOffsetY = 15.0F;
constexpr float kTopDoorVisualOffsetY = 10.0F;
constexpr float kStarterBottomWallThicknessOffset = 20.0F;

} // namespace

BaseRoom::BaseRoom(
    const glm::vec2 &absolutePosition,
    RoomType roomType,
    RoomPurpose purpose,
    const DoorConfig &doorConfig,
    const WallConfig &wallConfig
)
    : RectMapArea(
          absolutePosition,
          BaseRoom::ResolveRoomSprite(roomType),
          BaseRoom::ResolveRoomSize(roomType),
          wallConfig
      ),
      m_RoomType(roomType),
      m_Purpose(purpose),
      m_DoorConfig(doorConfig) {
    this->BuildDoors();
}

void BaseRoom::Update() {
    for (const auto &child : this->GetChildren()) {
        const std::shared_ptr<IStateful> statefulChild =
            std::dynamic_pointer_cast<IStateful>(child);
        if (statefulChild != nullptr) {
            statefulChild->Update();
        }
    }
}

bool BaseRoom::IsPlayerInside(const glm::vec2 &playerPos) const {
    return this->IsPointInside(playerPos);
}

const std::vector<Collision::AxisAlignedBox> &BaseRoom::GetStaticColliders() const {
    return RectMapArea::GetStaticColliders();
}

std::vector<Collision::AxisAlignedBox> BaseRoom::GetDynamicColliders(
    const Collision::AxisAlignedBox *ignoreOverlapBox
) const {
    std::vector<Collision::AxisAlignedBox> dynamicColliders;
    Collision::CollisionSystem collisionSystem;

    for (const auto &door : this->m_Doors) {
        if (door == nullptr || door->IsOpen()) {
            continue;
        }

        const Collision::AxisAlignedBox doorBox = Collision::CollisionSystem::BuildBox(
            door->GetColliderCenter(),
            door->GetColliderSize()
        );
        if (ignoreOverlapBox != nullptr &&
            collisionSystem.IsOverlapping(doorBox, *ignoreOverlapBox)) {
            continue;
        }

        dynamicColliders.push_back(doorBox);
    }

    return dynamicColliders;
}

const std::vector<std::shared_ptr<Door>> &BaseRoom::GetDoors() const {
    return this->m_Doors;
}

const std::vector<std::shared_ptr<Character>> &BaseRoom::GetMobs() const {
    return this->m_Mobs;
}

void BaseRoom::AddMob(const std::shared_ptr<Character> &mob) {
    if (mob == nullptr) {
        return;
    }

    this->m_Mobs.push_back(mob);
    this->AddChild(mob);
}

void BaseRoom::OpenAllDoors() {
    for (const auto &door : this->m_Doors) {
        if (door != nullptr) {
            door->Open();
        }
    }
}

void BaseRoom::CloseAllDoors() {
    for (const auto &door : this->m_Doors) {
        if (door != nullptr) {
            door->Close();
        }
    }
}

void BaseRoom::OnPlayerEnter() {
}

void BaseRoom::OnPlayerLeave() {
}

glm::vec2 BaseRoom::GetRoomSize() const {
    return this->GetAreaSize();
}

RoomType BaseRoom::GetRoomType() const {
    return this->m_RoomType;
}

RoomPurpose BaseRoom::GetPurpose() const {
    return this->m_Purpose;
}

bool BaseRoom::HasPassageOnSide(DoorSide side) const {
    return this->GetWallSideConfig(side).hasOpening ||
           this->GetDoorSideConfig(side).hasDoor;
}

WallConfig BaseRoom::BuildWallConfigFromDoorConfig(
    const DoorConfig &doorConfig,
    float wallThickness
) {
    WallConfig wallConfig;
    wallConfig.top.thickness = wallThickness;
    wallConfig.right.thickness = wallThickness;
    wallConfig.bottom.thickness = wallThickness + kStarterBottomWallThicknessOffset;
    wallConfig.left.thickness = wallThickness;

    wallConfig.top.hasOpening = doorConfig.top.hasDoor;
    wallConfig.top.openingSize = doorConfig.top.hasDoor ?
        MapColliderConfig::kHorizontalDoorColliderSize.x :
        0.0F;
    wallConfig.right.hasOpening = doorConfig.right.hasDoor;
    wallConfig.right.openingSize = doorConfig.right.hasDoor ?
        MapColliderConfig::kVerticalDoorColliderSize.y :
        0.0F;
    wallConfig.bottom.hasOpening = doorConfig.bottom.hasDoor;
    wallConfig.bottom.openingSize = doorConfig.bottom.hasDoor ?
        MapColliderConfig::kHorizontalDoorColliderSize.x :
        0.0F;
    wallConfig.left.hasOpening = doorConfig.left.hasDoor;
    wallConfig.left.openingSize = doorConfig.left.hasDoor ?
        MapColliderConfig::kVerticalDoorColliderSize.y :
        0.0F;

    return wallConfig;
}

glm::vec2 BaseRoom::ResolveRoomSize(RoomType roomType) {
    switch (roomType) {
    
    case RoomType::ROOM_13_13:
        return {13.0F * MAP_PIXEL_PER_BLOCK, 14.5F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_15_15:
        return {15.0F * MAP_PIXEL_PER_BLOCK, 16.5F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_17_17:
        return {17.0F * MAP_PIXEL_PER_BLOCK, 18.5F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_17_23:
        return {17.0F * MAP_PIXEL_PER_BLOCK, 24.5F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_23_17:
        return {23.0F * MAP_PIXEL_PER_BLOCK, 18.5F * MAP_PIXEL_PER_BLOCK};
    }

    return {13.0F * MAP_PIXEL_PER_BLOCK, 14.5F * MAP_PIXEL_PER_BLOCK};
}

std::string BaseRoom::ResolveRoomSprite(RoomType roomType) {
    switch (roomType) {
    case RoomType::ROOM_13_13:
        return RESOURCE_DIR "/Map/Room/Room_13x13.png";

    case RoomType::ROOM_15_15:
        return RESOURCE_DIR "/Map/Room/Room_15x15.png";

    case RoomType::ROOM_17_17:
        return RESOURCE_DIR "/Map/Room/Room_17x17.png";

    case RoomType::ROOM_17_23:
        return RESOURCE_DIR "/Map/Room/Room_17x23.png";

    case RoomType::ROOM_23_17:
        return RESOURCE_DIR "/Map/Room/Room_23x17.png";
    }

    return RESOURCE_DIR "/Map/Room/Room_13x13.png";
}

Door::Visuals BaseRoom::BuildHorizontalDoorVisuals() {
    Door::Visuals visuals;
    visuals.closedIdle = std::make_shared<Util::Image>(kHorizontalClosedDoorSprite, false);
    return visuals;
}

Door::Visuals BaseRoom::BuildVerticalDoorVisuals() {
    Door::Visuals visuals;
    visuals.closedIdle = std::make_shared<Util::Image>(kVerticalClosedDoorSprite, false);
    return visuals;
}

glm::vec2 BaseRoom::BuildDoorPosition(const DoorBuildInfo &doorInfo) const {
    const glm::vec2 roomCenter = this->GetAbsoluteTranslation();
    const glm::vec2 roomSize = this->GetAreaSize();

    switch (doorInfo.side) {
    case DoorSide::Top:
        return {
            roomCenter.x + doorInfo.openingOffset,
            roomCenter.y + roomSize.y / 2.0F - doorInfo.renderSize.y / 2.0F
        };

    case DoorSide::Right:
        return {
            roomCenter.x + roomSize.x / 2.0F - doorInfo.renderSize.x / 2.0F,
            roomCenter.y + doorInfo.openingOffset
        };

    case DoorSide::Bottom:
        return {
            roomCenter.x + doorInfo.openingOffset,
            roomCenter.y - roomSize.y / 2.0F + doorInfo.renderSize.y / 2.0F
        };

    case DoorSide::Left:
        return {
            roomCenter.x - roomSize.x / 2.0F + doorInfo.renderSize.x / 2.0F,
            roomCenter.y + doorInfo.openingOffset
        };
    }

    return roomCenter;
}

const DoorSideConfig &BaseRoom::GetDoorSideConfig(DoorSide side) const {
    switch (side) {
    case DoorSide::Top:
        return this->m_DoorConfig.top;

    case DoorSide::Right:
        return this->m_DoorConfig.right;

    case DoorSide::Bottom:
        return this->m_DoorConfig.bottom;

    case DoorSide::Left:
        return this->m_DoorConfig.left;
    }

    return this->m_DoorConfig.bottom;
}

const WallSideConfig &BaseRoom::GetWallSideConfig(DoorSide side) const {
    switch (side) {
    case DoorSide::Top:
        return this->m_WallConfig.top;

    case DoorSide::Right:
        return this->m_WallConfig.right;

    case DoorSide::Bottom:
        return this->m_WallConfig.bottom;

    case DoorSide::Left:
        return this->m_WallConfig.left;
    }

    return this->m_WallConfig.bottom;
}

void BaseRoom::BuildDoors() {
    const DoorSide sides[] = {
        DoorSide::Top,
        DoorSide::Right,
        DoorSide::Bottom,
        DoorSide::Left
    };

    for (DoorSide side : sides) {
        const DoorSideConfig &doorSideConfig = this->GetDoorSideConfig(side);
        if (!doorSideConfig.hasDoor) {
            continue;
        }

        DoorBuildInfo doorInfo;
        doorInfo.side = side;
        doorInfo.startsOpen = doorSideConfig.startsOpen;
        doorInfo.openingOffset = this->GetWallSideConfig(side).openingOffset;

        switch (side) {
        case DoorSide::Top:
            doorInfo.colliderSize = MapColliderConfig::kHorizontalDoorColliderSize;
            doorInfo.renderSize = MapColliderConfig::kHorizontalDoorRenderSize;
            doorInfo.positionOffset = {0.0F, kTopDoorVisualOffsetY};
            doorInfo.visuals = BaseRoom::BuildHorizontalDoorVisuals();
            break;

        case DoorSide::Right:
            doorInfo.colliderSize = MapColliderConfig::kVerticalDoorColliderSize;
            doorInfo.renderSize = MapColliderConfig::kVerticalDoorRenderSize;
            doorInfo.visuals = BaseRoom::BuildVerticalDoorVisuals();
            break;

        case DoorSide::Bottom:
            doorInfo.colliderSize = MapColliderConfig::kHorizontalDoorColliderSize;
            doorInfo.renderSize = MapColliderConfig::kHorizontalDoorRenderSize;
            doorInfo.positionOffset = {0.0F, kBottomDoorVisualOffsetY};
            doorInfo.visuals = BaseRoom::BuildHorizontalDoorVisuals();
            break;

        case DoorSide::Left:
            doorInfo.colliderSize = MapColliderConfig::kVerticalDoorColliderSize;
            doorInfo.renderSize = MapColliderConfig::kVerticalDoorRenderSize;
            doorInfo.visuals = BaseRoom::BuildVerticalDoorVisuals();
            break;
        }

        const std::shared_ptr<Door> door = std::make_shared<Door>(
            this->BuildDoorPosition(doorInfo) + doorInfo.positionOffset,
            doorInfo.side,
            doorInfo.colliderSize,
            doorInfo.renderSize,
            doorInfo.visuals,
            doorInfo.startsOpen
        );
        door->SetColliderOffset(-doorInfo.positionOffset);

        this->m_Doors.push_back(door);
        this->AddChild(door);
    }
}
