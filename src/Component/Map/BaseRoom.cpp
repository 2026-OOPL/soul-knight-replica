#include "Component/Map/BaseRoom.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "Component/Map/MapColliderConfig.hpp"

namespace {

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

} // namespace

BaseRoom::BaseRoom(
    const std::string &resourcePath,
    const glm::vec2 &roomSize,
    RoomAssemblyConfig config
)
    : MapPiece(config.roomCenter, resourcePath),
      m_RoomSize(roomSize),
      m_Config(std::move(config)) {
    this->m_AbsoluteTransform.scale = SafeScaleForSize(this->m_Drawable, this->m_RoomSize);
    this->SetColliderSize(this->m_RoomSize);
    this->BuildWallColliders();
    this->m_CollisionSystem.SetStaticBlockingBoxes(this->m_WallColliders);
    this->m_CollisionSystem.SetBlockingBoxProvider(
        [this]() {
            return this->CollectClosedDoorColliders();
        }
    );
}

bool BaseRoom::IsPlayerInside(const glm::vec2 &playerPos) const {
    const glm::vec2 roomHalfSize = this->m_RoomSize / 2.0F;
    const glm::vec2 offset = playerPos - this->GetAbsoluteCooridinate();

    return std::abs(offset.x) <= roomHalfSize.x &&
           std::abs(offset.y) <= roomHalfSize.y;
}

Collision::MovementResult BaseRoom::ResolvePlayerMovement(
    const Collision::AxisAlignedBox &currentBox,
    const glm::vec2 &intendedDelta
) const {
    return this->m_CollisionSystem.ResolveMovement(currentBox, intendedDelta);
}

void BaseRoom::SetDoors(const std::vector<std::shared_ptr<Door>> &doors) {
    this->m_Doors = doors;
}

const std::vector<Collision::AxisAlignedBox> &BaseRoom::GetWallColliders() const {
    return this->m_WallColliders;
}

glm::vec2 BaseRoom::GetRoomSize() const {
    return this->m_RoomSize;
}

RoomPurpose BaseRoom::GetPurpose() const {
    return this->m_Config.purpose;
}

Collision::RoomBoundaryOpenings BaseRoom::BuildWallOpenings() const {
    Collision::RoomBoundaryOpenings openings;

    if (this->m_Config.boundaries.top.hasDoor) {
        openings.top.size = this->GetOpeningSizeForSide(DoorSide::Top);
        openings.top.offset = this->m_Config.boundaries.top.openingOffset;
    }

    if (this->m_Config.boundaries.right.hasDoor) {
        openings.right.size = this->GetOpeningSizeForSide(DoorSide::Right);
        openings.right.offset = this->m_Config.boundaries.right.openingOffset;
    }

    if (this->m_Config.boundaries.bottom.hasDoor) {
        openings.bottom.size = this->GetOpeningSizeForSide(DoorSide::Bottom);
        openings.bottom.offset = this->m_Config.boundaries.bottom.openingOffset;
    }

    if (this->m_Config.boundaries.left.hasDoor) {
        openings.left.size = this->GetOpeningSizeForSide(DoorSide::Left);
        openings.left.offset = this->m_Config.boundaries.left.openingOffset;
    }

    return openings;
}

std::vector<Collision::AxisAlignedBox> BaseRoom::CollectClosedDoorColliders() const {
    std::vector<Collision::AxisAlignedBox> closedDoorColliders;

    for (const auto &door : this->m_Doors) {
        if (door == nullptr || door->IsOpen()) {
            continue;
        }

        closedDoorColliders.push_back(Collision::CollisionSystem::BuildBox(
            door->GetAbsoluteCooridinate(),
            door->GetColliderSize()
        ));
    }

    return closedDoorColliders;
}

float BaseRoom::GetOpeningSizeForSide(DoorSide side) const {
    switch (side) {
    case DoorSide::Top:
        if (this->m_Config.boundaries.top.openingSize > 0.0F) {
            return this->m_Config.boundaries.top.openingSize;
        }
        return MapColliderConfig::kHorizontalDoorColliderSize.x;

    case DoorSide::Right:
        if (this->m_Config.boundaries.right.openingSize > 0.0F) {
            return this->m_Config.boundaries.right.openingSize;
        }
        return MapColliderConfig::kVerticalDoorColliderSize.y;

    case DoorSide::Bottom:
        if (this->m_Config.boundaries.bottom.openingSize > 0.0F) {
            return this->m_Config.boundaries.bottom.openingSize;
        }
        return MapColliderConfig::kHorizontalDoorColliderSize.x;

    case DoorSide::Left:
        if (this->m_Config.boundaries.left.openingSize > 0.0F) {
            return this->m_Config.boundaries.left.openingSize;
        }
        return MapColliderConfig::kVerticalDoorColliderSize.y;
    }

    return 0.0F;
}

void BaseRoom::BuildWallColliders() {
    this->m_WallColliders = Collision::BuildRoomBoundaryBoxes(
        this->GetAbsoluteCooridinate(),
        this->m_RoomSize,
        this->m_Config.wallThickness,
        this->BuildWallOpenings()
    );
}
