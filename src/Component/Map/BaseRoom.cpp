#include "Component/Map/BaseRoom.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "Common/Constants.hpp"
#include "Component/Map/MapColliderConfig.hpp"
#include "Util/Image.hpp"

namespace {

constexpr char kHorizontalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_5x2.png";
constexpr char kVerticalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_1x5.png";
constexpr float kBottomDoorVisualOffsetY = 15.0F; // 不能刪除門會歪掉
constexpr float kTopDoorVisualOffsetY = 10.0F; // 同上

glm::vec2 SafeScaleForSize(
    const std::shared_ptr<Core::Drawable> &drawable,
    const glm::vec2 &roomSize
) {
    if (drawable == nullptr) {
        return {1.0F, 1.0F};
    }

    const glm::vec2 drawableSize = drawable->GetSize();
    return {
        roomSize.x / std::max(drawableSize.x, 1.0F),
        roomSize.y / std::max(drawableSize.y, 1.0F)
    };
}

std::vector<Collision::AxisAlignedBox> BuildHorizontalWallSegments(
    const glm::vec2 &roomCenter,
    float wallY,
    float wallWidth,
    float wallThickness,
    const Collision::WallOpening &opening
) {
    if (wallWidth <= 0.0F || wallThickness <= 0.0F) {
        return {};
    }

    const float openingWidth = std::clamp(opening.size, 0.0F, wallWidth);
    if (openingWidth <= 0.0F) {
        return {
            Collision::CollisionSystem::BuildBox(
                {roomCenter.x, wallY},
                {wallWidth, wallThickness}
            )
        };
    }

    const float wallLeft = roomCenter.x - wallWidth / 2.0F;
    const float wallRight = roomCenter.x + wallWidth / 2.0F;
    const float openingHalfWidth = openingWidth / 2.0F;
    const float openingCenterX = std::clamp(
        roomCenter.x + opening.offset,
        wallLeft + openingHalfWidth,
        wallRight - openingHalfWidth
    );
    const float leftWidth = std::max(0.0F, openingCenterX - openingHalfWidth - wallLeft);
    const float rightWidth = std::max(0.0F, wallRight - openingCenterX - openingHalfWidth);
    std::vector<Collision::AxisAlignedBox> segments;

    if (leftWidth > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallLeft + leftWidth / 2.0F, wallY},
            {leftWidth, wallThickness}
        ));
    }

    if (rightWidth > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallRight - rightWidth / 2.0F, wallY},
            {rightWidth, wallThickness}
        ));
    }

    return segments;
}

std::vector<Collision::AxisAlignedBox> BuildVerticalWallSegments(
    const glm::vec2 &roomCenter,
    float wallX,
    float wallHeight,
    float wallThickness,
    const Collision::WallOpening &opening
) {
    if (wallHeight <= 0.0F || wallThickness <= 0.0F) {
        return {};
    }

    const float openingHeight = std::clamp(opening.size, 0.0F, wallHeight);
    if (openingHeight <= 0.0F) {
        return {
            Collision::CollisionSystem::BuildBox(
                {wallX, roomCenter.y},
                {wallThickness, wallHeight}
            )
        };
    }

    const float wallBottom = roomCenter.y - wallHeight / 2.0F;
    const float wallTop = roomCenter.y + wallHeight / 2.0F;
    const float openingHalfHeight = openingHeight / 2.0F;
    const float openingCenterY = std::clamp(
        roomCenter.y + opening.offset,
        wallBottom + openingHalfHeight,
        wallTop - openingHalfHeight
    );
    const float bottomHeight = std::max(0.0F, openingCenterY - openingHalfHeight - wallBottom);
    const float topHeight = std::max(0.0F, wallTop - openingCenterY - openingHalfHeight);
    std::vector<Collision::AxisAlignedBox> segments;

    if (bottomHeight > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallX, wallBottom + bottomHeight / 2.0F},
            {wallThickness, bottomHeight}
        ));
    }

    if (topHeight > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallX, wallTop - topHeight / 2.0F},
            {wallThickness, topHeight}
        ));
    }

    return segments;
}

} // namespace

BaseRoom::BaseRoom(
    const glm::vec2 &absolutePosition,
    RoomType roomType,
    RoomPurpose purpose,
    const DoorConfig &doorConfig,
    const WallConfig &wallConfig
)
    : MapPiece(
          absolutePosition,
          BaseRoom::ResolveRoomSprite(roomType)
      ),
      m_RoomSize(BaseRoom::ResolveRoomSize(roomType)),
      m_RoomType(roomType),
      m_Purpose(purpose),
      m_DoorConfig(doorConfig),
      m_WallConfig(wallConfig) {
    this->m_AbsoluteTransform.scale = SafeScaleForSize(this->m_Drawable, this->m_RoomSize);
    this->SetColliderSize(this->m_RoomSize);
    this->BuildDoors();
    this->BuildWallColliders();
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
    const glm::vec2 roomHalfSize = this->m_RoomSize / 2.0F;
    const glm::vec2 offset = playerPos - this->GetAbsoluteCooridinate();

    return std::abs(offset.x) <= roomHalfSize.x &&
           std::abs(offset.y) <= roomHalfSize.y;
}

const std::vector<Collision::AxisAlignedBox> &BaseRoom::GetStaticColliders() const {
    return this->m_StaticColliders;
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

const std::vector<std::shared_ptr<Mob>> &BaseRoom::GetMobs() const {
    return this->m_Mobs;
}

void BaseRoom::AddMob(const std::shared_ptr<Mob> &mob) {
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
    return this->m_RoomSize;
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
    float bottomOffest=20.0F;
    WallConfig wallConfig;
    wallConfig.top.thickness = wallThickness;
    wallConfig.right.thickness = wallThickness;
    wallConfig.bottom.thickness = wallThickness+bottomOffest;
    wallConfig.left.thickness = wallThickness;
    wallConfig.top.hasOpening = doorConfig.top.hasDoor;
    wallConfig.right.hasOpening = doorConfig.right.hasDoor;
    wallConfig.bottom.hasOpening = doorConfig.bottom.hasDoor;
    wallConfig.left.hasOpening = doorConfig.left.hasDoor;

    if (!doorConfig.top.hasDoor) {
        wallConfig.top.openingOffset = 0.0F;
    }
    if (!doorConfig.right.hasDoor) {
        wallConfig.right.openingOffset = 0.0F;
    }
    if (!doorConfig.bottom.hasDoor) {
        wallConfig.bottom.openingOffset = 0.0F;
    }
    if (!doorConfig.left.hasDoor) {
        wallConfig.left.openingOffset = 0.0F;
    }

    return wallConfig;
}

glm::vec2 BaseRoom::ResolveRoomSize(RoomType roomType) {
    switch (roomType) {
    case RoomType::ROOM_13_13:
        return {13.0F * MAP_PIXEL_PER_BLOCK, 13.0F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_15_15:
        return {15.0F * MAP_PIXEL_PER_BLOCK, 15.0F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_17_17:
        return {17.0F * MAP_PIXEL_PER_BLOCK, 17.0F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_17_23:
        return {17.0F * MAP_PIXEL_PER_BLOCK, 23.0F * MAP_PIXEL_PER_BLOCK};

    case RoomType::ROOM_23_17:
        return {23.0F * MAP_PIXEL_PER_BLOCK, 17.0F * MAP_PIXEL_PER_BLOCK};
    }

    return {13.0F * MAP_PIXEL_PER_BLOCK, 13.0F * MAP_PIXEL_PER_BLOCK};
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
    visuals.closedIdle = std::make_shared<Util::Image>(kHorizontalClosedDoorSprite);
    return visuals;
}

Door::Visuals BaseRoom::BuildVerticalDoorVisuals() {
    Door::Visuals visuals;
    visuals.closedIdle = std::make_shared<Util::Image>(kVerticalClosedDoorSprite);
    return visuals;
}

glm::vec2 BaseRoom::BuildDoorPosition(const DoorBuildInfo &doorInfo) const {
    const glm::vec2 roomCenter = this->GetAbsoluteCooridinate();

    switch (doorInfo.side) {
    case DoorSide::Top:
        return {
            roomCenter.x + doorInfo.openingOffset,
            roomCenter.y + this->m_RoomSize.y / 2.0F - doorInfo.renderSize.y / 2.0F
        };

    case DoorSide::Right:
        return {
            roomCenter.x + this->m_RoomSize.x / 2.0F - doorInfo.renderSize.x / 2.0F,
            roomCenter.y + doorInfo.openingOffset
        };

    case DoorSide::Bottom:
        return {
            roomCenter.x + doorInfo.openingOffset,
            roomCenter.y - this->m_RoomSize.y / 2.0F + doorInfo.renderSize.y / 2.0F
        };

    case DoorSide::Left:
        return {
            roomCenter.x - this->m_RoomSize.x / 2.0F + doorInfo.renderSize.x / 2.0F,
            roomCenter.y + doorInfo.openingOffset
        };
    }

    return roomCenter;
}

Collision::RoomBoundaryOpenings BaseRoom::BuildWallOpenings() const {
    Collision::RoomBoundaryOpenings openings;

    if (this->m_WallConfig.top.hasOpening || this->m_DoorConfig.top.hasDoor) {
        openings.top.size = this->GetOpeningSizeForSide(DoorSide::Top);
        openings.top.offset = this->m_WallConfig.top.openingOffset;
    }

    if (this->m_WallConfig.right.hasOpening || this->m_DoorConfig.right.hasDoor) {
        openings.right.size = this->GetOpeningSizeForSide(DoorSide::Right);
        openings.right.offset = this->m_WallConfig.right.openingOffset;
    }

    if (this->m_WallConfig.bottom.hasOpening || this->m_DoorConfig.bottom.hasDoor) {
        openings.bottom.size = this->GetOpeningSizeForSide(DoorSide::Bottom);
        openings.bottom.offset = this->m_WallConfig.bottom.openingOffset;
    }

    if (this->m_WallConfig.left.hasOpening || this->m_DoorConfig.left.hasDoor) {
        openings.left.size = this->GetOpeningSizeForSide(DoorSide::Left);
        openings.left.offset = this->m_WallConfig.left.openingOffset;
    }

    return openings;
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

float BaseRoom::GetOpeningSizeForSide(DoorSide side) const {
    switch (side) {
    case DoorSide::Top:
    case DoorSide::Bottom:
        return MapColliderConfig::kHorizontalDoorColliderSize.x;

    case DoorSide::Right:
    case DoorSide::Left:
        return MapColliderConfig::kVerticalDoorColliderSize.y;
    }

    return 0.0F;
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

void BaseRoom::BuildWallColliders() {
    this->m_StaticColliders.clear();

    const glm::vec2 roomCenter = this->GetAbsoluteCooridinate();
    const glm::vec2 roomHalfSize = this->m_RoomSize / 2.0F;
    const Collision::RoomBoundaryOpenings openings = this->BuildWallOpenings();

    const std::vector<Collision::AxisAlignedBox> topWalls = BuildHorizontalWallSegments(
        roomCenter,
        roomCenter.y + roomHalfSize.y - this->m_WallConfig.top.thickness / 2.0F,
        this->m_RoomSize.x,
        this->m_WallConfig.top.thickness,
        openings.top
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        topWalls.begin(),
        topWalls.end()
    );

    const std::vector<Collision::AxisAlignedBox> rightWalls = BuildVerticalWallSegments(
        roomCenter,
        roomCenter.x + roomHalfSize.x - this->m_WallConfig.right.thickness / 2.0F,
        this->m_RoomSize.y,
        this->m_WallConfig.right.thickness,
        openings.right
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        rightWalls.begin(),
        rightWalls.end()
    );

    const std::vector<Collision::AxisAlignedBox> bottomWalls = BuildHorizontalWallSegments(
        roomCenter,
        roomCenter.y - roomHalfSize.y + this->m_WallConfig.bottom.thickness / 2.0F,
        this->m_RoomSize.x,
        this->m_WallConfig.bottom.thickness,
        openings.bottom
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        bottomWalls.begin(),
        bottomWalls.end()
    );

    const std::vector<Collision::AxisAlignedBox> leftWalls = BuildVerticalWallSegments(
        roomCenter,
        roomCenter.x - roomHalfSize.x + this->m_WallConfig.left.thickness / 2.0F,
        this->m_RoomSize.y,
        this->m_WallConfig.left.thickness,
        openings.left
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        leftWalls.begin(),
        leftWalls.end()
    );
}
