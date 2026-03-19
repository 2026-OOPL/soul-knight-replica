#include "Component/Collision/CollisionSystem.hpp"

#include <algorithm>
#include <utility>

#include "Component/Map/MapPiece.hpp"

namespace {

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

} // namespace

namespace Collision {

AxisAlignedBox CollisionSystem::BuildBox(const glm::vec2 &center, const glm::vec2 &size) {
    return {
        center,
        size
    };
}

void CollisionSystem::SetStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes) {
    m_StaticBlockingBoxes = blockingBoxes;
}

void CollisionSystem::SetBlockingBoxProvider(BlockingBoxProvider blockingBoxProvider) {
    m_BlockingBoxProvider = std::move(blockingBoxProvider);
}

std::vector<AxisAlignedBox> CollisionSystem::GetBlockingBoxes() const {
    std::vector<AxisAlignedBox> blockingBoxes = m_StaticBlockingBoxes;

    if (m_BlockingBoxProvider) {
        std::vector<AxisAlignedBox> dynamicBoxes = m_BlockingBoxProvider();

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

    if (intendedDelta.x != 0.0F) {
        AxisAlignedBox horizontalCandidate = resolvedBox;
        horizontalCandidate.center.x += intendedDelta.x;

        if (IsBoxBlocked(horizontalCandidate, blockingBoxes, *this)) {
            result.blockedX = true;
        } else {
            result.resolvedDelta.x = intendedDelta.x;
            resolvedBox.center.x += intendedDelta.x;
        }
    }

    if (intendedDelta.y != 0.0F) {
        AxisAlignedBox verticalCandidate = resolvedBox;
        verticalCandidate.center.y += intendedDelta.y;

        if (IsBoxBlocked(verticalCandidate, blockingBoxes, *this)) {
            result.blockedY = true;
        } else {
            result.resolvedDelta.y = intendedDelta.y;
        }
    }

    return result;
}

std::vector<AxisAlignedBox> BuildWallBoxes(
    const std::vector<std::shared_ptr<MapPiece>> &pieces
) {
    std::vector<AxisAlignedBox> wallBoxes;

    for (const auto &piece : pieces) {
        if (piece == nullptr || !piece->IsWall()) {
            continue;
        }

        wallBoxes.push_back(CollisionSystem::BuildBox(
            piece->GetCooridinate(),
            piece->GetColliderSize()
        ));
    }

    return wallBoxes;
}

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness
) {
    const float safeWallThickness = std::max(0.0F, wallThickness);
    const glm::vec2 roomHalfSize = roomSize / 2.0F;
    const float halfWallThickness = safeWallThickness / 2.0F;

    return {
        CollisionSystem::BuildBox(
            {roomCenter.x, roomCenter.y + roomHalfSize.y - halfWallThickness},
            {roomSize.x, safeWallThickness}
        ),
        CollisionSystem::BuildBox(
            {roomCenter.x, roomCenter.y - roomHalfSize.y + halfWallThickness},
            {roomSize.x, safeWallThickness}
        ),
        CollisionSystem::BuildBox(
            {roomCenter.x - roomHalfSize.x + halfWallThickness, roomCenter.y},
            {safeWallThickness, roomSize.y}
        ),
        CollisionSystem::BuildBox(
            {roomCenter.x + roomHalfSize.x - halfWallThickness, roomCenter.y},
            {safeWallThickness, roomSize.y}
        )
    };
}

} // namespace Collision
