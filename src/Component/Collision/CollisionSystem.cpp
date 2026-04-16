#include <algorithm>
#include <cmath>
#include <utility>

#include "Component/Collision/CollisionDispatcher.hpp"
#include "Component/Collision/CollisionPrimitiveBuilder.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Collision/ICollidable.hpp"

namespace {

constexpr float kMaxCollisionSubstepDistance = 8.0F;

Collision::CollisionFilter MakeWorldBlockingFilter() {
    Collision::CollisionFilter filter;
    filter.layer = Collision::CollisionLayer::World;
    filter.mask = Collision::kAllCollisionLayers;
    filter.blocking = true;
    return filter;
}

bool IsInteractionEnabled(
    const Collision::CollisionPrimitive &lhs,
    const Collision::CollisionPrimitive &rhs,
    const Collision::CollisionSystem &collisionSystem
) {
    if (lhs.box.size.x <= 0.0F || lhs.box.size.y <= 0.0F ||
        rhs.box.size.x <= 0.0F || rhs.box.size.y <= 0.0F) {
        return false;
    }

    return collisionSystem.ShouldInteract(lhs.filter, rhs.filter);
}

bool IsBlockingPrimitive(
    const Collision::CollisionPrimitive &candidate,
    const Collision::CollisionPrimitive &blockingPrimitive,
    const Collision::CollisionQueryOptions &options,
    const Collision::CollisionSystem &collisionSystem
) {
    if (!blockingPrimitive.filter.blocking) {
        return false;
    }

    if (options.ignoreOwner != nullptr &&
        blockingPrimitive.owner == options.ignoreOwner) {
        return false;
    }

    if (!Collision::MatchesMask(blockingPrimitive.filter.layer, options.blockerMask)) {
        return false;
    }

    return IsInteractionEnabled(candidate, blockingPrimitive, collisionSystem);
}

bool ArePrimitivesBlocked(
    const std::vector<Collision::CollisionPrimitive> &candidates,
    const std::vector<Collision::CollisionPrimitive> &blockingPrimitives,
    const Collision::CollisionQueryOptions &options,
    const Collision::CollisionSystem &collisionSystem,
    ICollidable **blockingOwner = nullptr
) {
    for (const auto &candidate : candidates) {
        for (const auto &blockingPrimitive : blockingPrimitives) {
            if (!IsBlockingPrimitive(candidate, blockingPrimitive, options, collisionSystem)) {
                continue;
            }

            if (!collisionSystem.IsOverlapping(candidate.box, blockingPrimitive.box)) {
                continue;
            }

            if (blockingOwner != nullptr) {
                *blockingOwner = blockingPrimitive.owner;
            }
            return true;
        }
    }

    return false;
}

void OffsetPrimitives(
    std::vector<Collision::CollisionPrimitive> &primitives,
    const glm::vec2 &delta
) {
    for (auto &primitive : primitives) {
        primitive.box.center += delta;
    }
}

Collision::MovementResult ResolvePrimitivesMovement(
    std::vector<Collision::CollisionPrimitive> primitives,
    const glm::vec2 &intendedDelta,
    const std::vector<Collision::CollisionPrimitive> &blockingPrimitives,
    const Collision::CollisionQueryOptions &options,
    const Collision::CollisionSystem &collisionSystem
) {
    Collision::MovementResult result;
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
            std::vector<Collision::CollisionPrimitive> horizontalCandidate = primitives;
            OffsetPrimitives(horizontalCandidate, {stepDelta.x, 0.0F});

            if (ArePrimitivesBlocked(
                    horizontalCandidate,
                    blockingPrimitives,
                    options,
                    collisionSystem,
                    &result.blockingOwnerX
                )) {
                result.blockedX = true;
            } else {
                result.resolvedDelta.x += stepDelta.x;
                primitives = std::move(horizontalCandidate);
            }
        }

        if (stepDelta.y != 0.0F) {
            std::vector<Collision::CollisionPrimitive> verticalCandidate = primitives;
            OffsetPrimitives(verticalCandidate, {0.0F, stepDelta.y});

            if (ArePrimitivesBlocked(
                    verticalCandidate,
                    blockingPrimitives,
                    options,
                    collisionSystem,
                    &result.blockingOwnerY
                )) {
                result.blockedY = true;
            } else {
                result.resolvedDelta.y += stepDelta.y;
                primitives = std::move(verticalCandidate);
            }
        }
    }

    return result;
}

} // namespace

namespace Collision {

AxisAlignedBox CollisionSystem::BuildBox(const glm::vec2 &center, const glm::vec2 &size) {
    return CollisionPrimitiveBuilder::BuildBox(center, size);
}

CollisionPrimitive CollisionSystem::BuildPrimitive(
    const glm::vec2 &origin,
    const CollisionBox &box,
    ICollidable *owner
) {
    return CollisionPrimitiveBuilder::BuildPrimitive(origin, box, owner);
}

CollisionPrimitive CollisionSystem::BuildStaticPrimitive(
    const AxisAlignedBox &box,
    const CollisionFilter &filter
) {
    return CollisionPrimitiveBuilder::BuildStaticPrimitive(box, filter);
}

std::vector<CollisionPrimitive> CollisionSystem::BuildCollisionPrimitives(
    const ICollidable &body
) {
    return CollisionPrimitiveBuilder::BuildCollisionPrimitives(body);
}

std::vector<CollisionPrimitive> CollisionSystem::BuildCollisionPrimitives(
    const ICollidable &body,
    const glm::vec2 &origin
) {
    return CollisionPrimitiveBuilder::BuildCollisionPrimitives(body, origin);
}

void CollisionSystem::AddStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes) {
    const CollisionFilter worldFilter = MakeWorldBlockingFilter();

    for (const auto &blockingBox : blockingBoxes) {
        this->m_StaticBlockingPrimitives.push_back(BuildStaticPrimitive(blockingBox, worldFilter));
    }
}

void CollisionSystem::SetStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes) {
    this->m_StaticBlockingPrimitives.clear();
    this->AddStaticBlockingBoxes(blockingBoxes);
}

void CollisionSystem::SetBlockingBoxProvider(BlockingBoxProvider blockingBoxProvider) {
    if (!blockingBoxProvider) {
        this->m_BlockingPrimitiveProvider = nullptr;
        return;
    }

    this->m_BlockingPrimitiveProvider =
        [blockingBoxProvider = std::move(blockingBoxProvider)]() {
            const CollisionFilter worldFilter = MakeWorldBlockingFilter();
            std::vector<CollisionPrimitive> primitives;

            for (const AxisAlignedBox &box : blockingBoxProvider()) {
                primitives.push_back(CollisionSystem::BuildStaticPrimitive(box, worldFilter));
            }

            return primitives;
        };
}

void CollisionSystem::SetBlockingPrimitiveProvider(
    BlockingPrimitiveProvider blockingPrimitiveProvider
) {
    this->m_BlockingPrimitiveProvider = std::move(blockingPrimitiveProvider);
}

void CollisionSystem::SetDynamicBodyProvider(DynamicBodyProvider dynamicBodyProvider) {
    this->m_DynamicBodyProvider = std::move(dynamicBodyProvider);
}

std::vector<AxisAlignedBox> CollisionSystem::GetBlockingBoxes() const {
    std::vector<AxisAlignedBox> blockingBoxes;

    for (const auto &primitive : this->GetBlockingPrimitives()) {
        blockingBoxes.push_back(primitive.box);
    }

    return blockingBoxes;
}

std::vector<CollisionPrimitive> CollisionSystem::GetBlockingPrimitives() const {
    std::vector<CollisionPrimitive> blockingPrimitives = this->m_StaticBlockingPrimitives;

    if (this->m_BlockingPrimitiveProvider) {
        const std::vector<CollisionPrimitive> dynamicPrimitives =
            this->m_BlockingPrimitiveProvider();

        blockingPrimitives.insert(
            blockingPrimitives.end(),
            dynamicPrimitives.begin(),
            dynamicPrimitives.end()
        );
    }

    return blockingPrimitives;
}

std::vector<ICollidable *> CollisionSystem::GetDynamicBodies() const {
    if (!this->m_DynamicBodyProvider) {
        return {};
    }

    return this->m_DynamicBodyProvider();
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

bool CollisionSystem::ShouldInteract(
    const CollisionFilter &lhs,
    const CollisionFilter &rhs
) const {
    if (lhs.layer == CollisionLayer::None || rhs.layer == CollisionLayer::None) {
        return false;
    }

    return MatchesMask(rhs.layer, lhs.mask) &&
           MatchesMask(lhs.layer, rhs.mask);
}

bool CollisionSystem::IsBlocked(const AxisAlignedBox &box) const {
    return this->IsBlocked(box, this->GetBlockingPrimitives());
}

bool CollisionSystem::IsBlocked(
    const AxisAlignedBox &box,
    const std::vector<CollisionPrimitive> &blockingPrimitives,
    const CollisionQueryOptions &options
) const {
    CollisionPrimitive candidate;
    candidate.box = box;
    candidate.filter.layer = CollisionLayer::Prop;
    candidate.filter.mask = kAllCollisionLayers;

    return ArePrimitivesBlocked(
        {candidate},
        blockingPrimitives,
        options,
        *this
    );
}

bool CollisionSystem::CanOccupy(
    const ICollidable &body,
    const glm::vec2 &targetOrigin,
    const CollisionQueryOptions &options
) const {
    return this->CanOccupy(body, targetOrigin, this->GetBlockingPrimitives(), options);
}

bool CollisionSystem::CanOccupy(
    const ICollidable &body,
    const glm::vec2 &targetOrigin,
    const std::vector<CollisionPrimitive> &blockingPrimitives,
    const CollisionQueryOptions &options
) const {
    const std::vector<CollisionPrimitive> primitives =
        BuildCollisionPrimitives(body, targetOrigin);

    return !ArePrimitivesBlocked(primitives, blockingPrimitives, options, *this);
}

MovementResult CollisionSystem::ResolveMovement(
    const AxisAlignedBox &currentBox,
    const glm::vec2 &intendedDelta
) const {
    return this->ResolveMovement(currentBox, intendedDelta, this->GetBlockingPrimitives());
}

MovementResult CollisionSystem::ResolveMovement(
    const AxisAlignedBox &currentBox,
    const glm::vec2 &intendedDelta,
    const std::vector<CollisionPrimitive> &blockingPrimitives,
    const CollisionQueryOptions &options
) const {
    CollisionPrimitive primitive;
    primitive.box = currentBox;
    primitive.filter.layer = CollisionLayer::Prop;
    primitive.filter.mask = kAllCollisionLayers;

    return ResolvePrimitivesMovement(
        {primitive},
        intendedDelta,
        blockingPrimitives,
        options,
        *this
    );
}

MovementResult CollisionSystem::ResolveMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta,
    const CollisionQueryOptions &options
) const {
    return this->ResolveMovement(body, intendedDelta, this->GetBlockingPrimitives(), options);
}

MovementResult CollisionSystem::ResolveMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta,
    const std::vector<CollisionPrimitive> &blockingPrimitives,
    const CollisionQueryOptions &options
) const {
    std::vector<CollisionPrimitive> primitives = BuildCollisionPrimitives(body);
    if (primitives.empty()) {
        return {};
    }

    return ResolvePrimitivesMovement(
        std::move(primitives),
        intendedDelta,
        blockingPrimitives,
        options,
        *this
    );
}

MovementResult CollisionSystem::PredictMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta,
    const CollisionQueryOptions &options
) const {
    return this->ResolveMovement(body, intendedDelta, options);
}

MovementResult CollisionSystem::PredictMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta,
    const std::vector<CollisionPrimitive> &blockingPrimitives,
    const CollisionQueryOptions &options
) const {
    return this->ResolveMovement(body, intendedDelta, blockingPrimitives, options);
}

void CollisionSystem::DispatchCollisions() {
    this->DispatchCollisions(this->GetBlockingPrimitives(), this->GetDynamicBodies());
}

void CollisionSystem::DispatchCollisions(
    const std::vector<CollisionPrimitive> &staticPrimitives,
    const std::vector<ICollidable *> &dynamicBodies
) const {
    CollisionDispatcher::Dispatch(*this, staticPrimitives, dynamicBodies);
}

} // namespace Collision
