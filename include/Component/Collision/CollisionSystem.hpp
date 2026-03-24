#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP

#include <functional>
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

class MapPiece;

namespace Collision {

struct AxisAlignedBox {
    glm::vec2 center = {0.0F, 0.0F};
    glm::vec2 size = {0.0F, 0.0F};
};

struct MovementResult {
    glm::vec2 resolvedDelta = {0.0F, 0.0F};
    bool blockedX = false;
    bool blockedY = false;
};

class CollisionSystem {
public:
    using BlockingBoxProvider = std::function<std::vector<AxisAlignedBox>()>;

    static AxisAlignedBox BuildBox(const glm::vec2 &center, const glm::vec2 &size);

    void SetStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes);
    void SetBlockingBoxProvider(BlockingBoxProvider blockingBoxProvider);

    std::vector<AxisAlignedBox> GetBlockingBoxes() const;

    bool IsOverlapping(const AxisAlignedBox &lhs, const AxisAlignedBox &rhs) const;
    bool IsBlocked(const AxisAlignedBox &box) const;

    MovementResult ResolveMovement(
        const AxisAlignedBox &currentBox,
        const glm::vec2 &intendedDelta
    ) const;

private:
    std::vector<AxisAlignedBox> m_StaticBlockingBoxes;
    BlockingBoxProvider m_BlockingBoxProvider;
};

std::vector<AxisAlignedBox> BuildWallBoxes(
    const std::vector<std::shared_ptr<MapPiece>> &pieces
);

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness,
    const glm::vec2 &doorOpeningSize = {0.0F, 0.0F}
);

} // namespace Collision

#endif
