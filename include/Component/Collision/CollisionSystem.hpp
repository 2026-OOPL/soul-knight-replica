#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP

#include <functional>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/RoomBoundaryBuilder.hpp"
#include "Component/Collision/CollisionTypes.hpp"

class ICollidable;

namespace Collision {

class CollisionSystem {
public:
    using BlockingBoxProvider = std::function<std::vector<AxisAlignedBox>()>;
    using BlockingPrimitiveProvider = std::function<std::vector<CollisionPrimitive>()>;
    using DynamicBodyProvider = std::function<std::vector<ICollidable *>()>;

    static AxisAlignedBox BuildBox(const glm::vec2 &center, const glm::vec2 &size);
    static CollisionPrimitive BuildPrimitive(
        const glm::vec2 &origin,
        const CollisionBox &box,
        ICollidable *owner = nullptr
    );
    static CollisionPrimitive BuildStaticPrimitive(
        const AxisAlignedBox &box,
        const CollisionFilter &filter
    );
    static std::vector<CollisionPrimitive> BuildCollisionPrimitives(const ICollidable &body);
    static std::vector<CollisionPrimitive> BuildCollisionPrimitives(
        const ICollidable &body,
        const glm::vec2 &origin
    );

    void AddStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes);
    void SetStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes);
    void SetBlockingBoxProvider(BlockingBoxProvider blockingBoxProvider);
    void SetBlockingPrimitiveProvider(BlockingPrimitiveProvider blockingPrimitiveProvider);
    void SetDynamicBodyProvider(DynamicBodyProvider dynamicBodyProvider);

    std::vector<AxisAlignedBox> GetBlockingBoxes() const;
    std::vector<CollisionPrimitive> GetBlockingPrimitives() const;
    std::vector<ICollidable *> GetDynamicBodies() const;

    bool IsOverlapping(const AxisAlignedBox &lhs, const AxisAlignedBox &rhs) const;
    bool ShouldInteract(const CollisionFilter &lhs, const CollisionFilter &rhs) const;
    bool IsBlocked(const AxisAlignedBox &box) const;
    bool IsBlocked(
        const AxisAlignedBox &box,
        const std::vector<CollisionPrimitive> &blockingPrimitives,
        const CollisionQueryOptions &options = {}
    ) const;
    bool CanOccupy(
        const ICollidable &body,
        const glm::vec2 &targetOrigin,
        const CollisionQueryOptions &options = {}
    ) const;
    bool CanOccupy(
        const ICollidable &body,
        const glm::vec2 &targetOrigin,
        const std::vector<CollisionPrimitive> &blockingPrimitives,
        const CollisionQueryOptions &options = {}
    ) const;

    MovementResult ResolveMovement(
        const AxisAlignedBox &currentBox,
        const glm::vec2 &intendedDelta
    ) const;
    MovementResult ResolveMovement(
        const AxisAlignedBox &currentBox,
        const glm::vec2 &intendedDelta,
        const std::vector<CollisionPrimitive> &blockingPrimitives,
        const CollisionQueryOptions &options = {}
    ) const;
    MovementResult ResolveMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta,
        const CollisionQueryOptions &options = {}
    ) const;
    MovementResult ResolveMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta,
        const std::vector<CollisionPrimitive> &blockingPrimitives,
        const CollisionQueryOptions &options = {}
    ) const;
    MovementResult PredictMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta,
        const CollisionQueryOptions &options = {}
    ) const;
    MovementResult PredictMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta,
        const std::vector<CollisionPrimitive> &blockingPrimitives,
        const CollisionQueryOptions &options = {}
    ) const;

    void DispatchCollisions();
    void DispatchCollisions(
        const std::vector<CollisionPrimitive> &staticPrimitives,
        const std::vector<ICollidable *> &dynamicBodies
    ) const;

private:
    std::vector<CollisionPrimitive> m_StaticBlockingPrimitives;
    BlockingPrimitiveProvider m_BlockingPrimitiveProvider;
    DynamicBodyProvider m_DynamicBodyProvider;
};

} // namespace Collision

#endif
