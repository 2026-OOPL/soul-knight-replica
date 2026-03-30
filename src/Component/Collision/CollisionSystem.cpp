#include "Component/Collision/CollisionSystem.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include "Component/ICollidiable.hpp"
#include "Component/Map/MapPiece.hpp"

namespace {

constexpr float kMaxCollisionSubstepDistance = 8.0F; // 單次碰撞解算的最大位移，避免大位移跨牆。

bool IsBoxBlocked(
    const Collision::AxisAlignedBox &candidate,
    const std::vector<Collision::AxisAlignedBox> &blockingBoxes,
    const Collision::CollisionSystem &collisionSystem
) {
    for (const auto &blockingBox : blockingBoxes) {
        if (collisionSystem.IsOverlapping(candidate, blockingBox)) {
            return true;
        }
    }

    return false;
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

namespace Collision {

AxisAlignedBox CollisionSystem::BuildBox(const glm::vec2 &center, const glm::vec2 &size) {
    return {
        center,
        size
    };
}

void CollisionSystem::AddStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes) {
    for (auto const &i : blockingBoxes) {
        this->m_StaticBlockingBoxes.push_back(i);
    }
}

void CollisionSystem::SetStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes) {
    this->m_StaticBlockingBoxes = blockingBoxes;
}

void CollisionSystem::SetBlockingBoxProvider(BlockingBoxProvider blockingBoxProvider) {
    this->m_BlockingBoxProvider = std::move(blockingBoxProvider);
}

std::vector<AxisAlignedBox> CollisionSystem::GetBlockingBoxes() const {
    std::vector<AxisAlignedBox> blockingBoxes = this->m_StaticBlockingBoxes;

    if (this->m_BlockingBoxProvider) {
        const std::vector<AxisAlignedBox> dynamicBoxes = this->m_BlockingBoxProvider();

        blockingBoxes.insert(
            blockingBoxes.end(),
            dynamicBoxes.begin(),
            dynamicBoxes.end()
        );
    }

    return blockingBoxes;
}

bool CollisionSystem::IsOverlapping(
    const AxisAlignedBox &lhs,
    const AxisAlignedBox &rhs
) const {
    const glm::vec2 lhsHalfSize = lhs.size / 2.0F;
    const glm::vec2 rhsHalfSize = rhs.size / 2.0F;

    const float lhsLeft = lhs.center.x - lhsHalfSize.x;
    const float lhsRight = lhs.center.x + lhsHalfSize.x;
    const float lhsTop = lhs.center.y + lhsHalfSize.y;
    const float lhsBottom = lhs.center.y - lhsHalfSize.y;

    const float rhsLeft = rhs.center.x - rhsHalfSize.x;
    const float rhsRight = rhs.center.x + rhsHalfSize.x;
    const float rhsTop = rhs.center.y + rhsHalfSize.y;
    const float rhsBottom = rhs.center.y - rhsHalfSize.y;

    return !(lhsRight <= rhsLeft || lhsLeft >= rhsRight ||
             lhsTop <= rhsBottom || lhsBottom >= rhsTop);
}

bool CollisionSystem::IsBlocked(const AxisAlignedBox &box) const {
    const std::vector<AxisAlignedBox> blockingBoxes = this->GetBlockingBoxes();

    return IsBoxBlocked(box, blockingBoxes, *this);
}

MovementResult CollisionSystem::ResolveMovement(
    const AxisAlignedBox &currentBox,
    const glm::vec2 &intendedDelta
) const {
    const std::vector<AxisAlignedBox> blockingBoxes = this->GetBlockingBoxes();
    MovementResult result;
    AxisAlignedBox resolvedBox = currentBox;
    const float maxComponentDelta = std::max(
        std::abs(intendedDelta.x),
        std::abs(intendedDelta.y)
    );
    const int stepCount = std::max(
        1,
        static_cast<int>(std::ceil(maxComponentDelta / kMaxCollisionSubstepDistance))
    );
    const glm::vec2 stepDelta = intendedDelta / static_cast<float>(stepCount);

    for (int step = 0; step < stepCount; ++step) {
        if (stepDelta.x != 0.0F) {
            AxisAlignedBox horizontalCandidate = resolvedBox;
            horizontalCandidate.center.x += stepDelta.x;

            if (IsBoxBlocked(horizontalCandidate, blockingBoxes, *this)) {
                result.blockedX = true;
            } else {
                result.resolvedDelta.x += stepDelta.x;
                resolvedBox.center.x += stepDelta.x;
            }
        }

        if (stepDelta.y != 0.0F) {
            AxisAlignedBox verticalCandidate = resolvedBox;
            verticalCandidate.center.y += stepDelta.y;

            if (IsBoxBlocked(verticalCandidate, blockingBoxes, *this)) {
                result.blockedY = true;
            } else {
                result.resolvedDelta.y += stepDelta.y;
                resolvedBox.center.y += stepDelta.y;
            }
        }
    }

    return result;
}

std::vector<AxisAlignedBox> BuildWallBoxes(
    const std::vector<std::shared_ptr<MapPiece>> &pieces
) {
    std::vector<AxisAlignedBox> wallBoxes;

    for (const auto &piece : pieces) {
        
        std::shared_ptr<ICollidable> collidable = std::dynamic_pointer_cast<ICollidable>(piece);
        if (collidable == nullptr) {
            continue;
        }

        wallBoxes.push_back(CollisionSystem::BuildBox(
            collidable->GetColliderCooridinate(),
            collidable->GetColliderSize()
        ));
    }

    return wallBoxes;
}

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness
) {
    return BuildRoomBoundaryBoxes(
        roomCenter,
        roomSize,
        wallThickness,
        RoomBoundaryOpenings{}
    );
}

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness,
    const RoomBoundaryOpenings &openings
) {
    const float safeWallThickness = std::max(0.0F, wallThickness);
    const glm::vec2 roomHalfSize = roomSize / 2.0F;
    const float halfWallThickness = safeWallThickness / 2.0F;
    std::vector<AxisAlignedBox> boundaryBoxes;

    const std::vector<AxisAlignedBox> topWalls = BuildHorizontalWallSegments(
        roomCenter,
        roomCenter.y + roomHalfSize.y - halfWallThickness,
        roomSize.x,
        safeWallThickness,
        openings.top
    );
    boundaryBoxes.insert(boundaryBoxes.end(), topWalls.begin(), topWalls.end());
    float bottomWallsOffest=20.0F;
    const std::vector<AxisAlignedBox> bottomWalls = BuildHorizontalWallSegments(
        roomCenter,
        roomCenter.y - roomHalfSize.y + halfWallThickness+bottomWallsOffest, //襪操
        roomSize.x,
        safeWallThickness,
        openings.bottom
    );
    boundaryBoxes.insert(boundaryBoxes.end(), bottomWalls.begin(), bottomWalls.end());

    const std::vector<AxisAlignedBox> leftWalls = BuildVerticalWallSegments(
        roomCenter,
        roomCenter.x - roomHalfSize.x + halfWallThickness,
        roomSize.y,
        safeWallThickness,
        openings.left
    );
    boundaryBoxes.insert(boundaryBoxes.end(), leftWalls.begin(), leftWalls.end());

    const std::vector<AxisAlignedBox> rightWalls = BuildVerticalWallSegments(
        roomCenter,
        roomCenter.x + roomHalfSize.x - halfWallThickness,
        roomSize.y,
        safeWallThickness,
        openings.right
    );
    boundaryBoxes.insert(boundaryBoxes.end(), rightWalls.begin(), rightWalls.end());

    return boundaryBoxes;
}

} // namespace Collision
