#include <algorithm>
#include <cmath>
#include <utility>

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

std::pair<glm::vec2, glm::vec2> ComputeSeparation(
    const Collision::AxisAlignedBox &lhs,
    const Collision::AxisAlignedBox &rhs
) {
    const glm::vec2 lhsHalf = lhs.size / 2.0F;
    const glm::vec2 rhsHalf = rhs.size / 2.0F;
    const glm::vec2 delta = lhs.center - rhs.center;

    const float overlapX = lhsHalf.x + rhsHalf.x - std::abs(delta.x);
    const float overlapY = lhsHalf.y + rhsHalf.y - std::abs(delta.y);

    if (overlapX <= 0.0F || overlapY <= 0.0F) {
        return {
            {0.0F, 0.0F},
            {0.0F, 0.0F}
        };
    }

    if (overlapX < overlapY) {
        const float sign = delta.x < 0.0F ? -1.0F : 1.0F;
        return {
            {sign, 0.0F},
            {sign * overlapX, 0.0F}
        };
    }

    const float sign = delta.y < 0.0F ? -1.0F : 1.0F;
    return {
        {0.0F, sign},
        {0.0F, sign * overlapY}
    };
}

Collision::CollisionSituationKind ResolveSituationKind(
    const Collision::CollisionPrimitive &lhs,
    const Collision::CollisionPrimitive &rhs
) {
    const bool isTrigger =
        lhs.filter.trigger ||
        rhs.filter.trigger ||
        lhs.type == Collision::CollisionBoxType::Trigger ||
        rhs.type == Collision::CollisionBoxType::Trigger;

    if (isTrigger) {
        return Collision::CollisionSituationKind::Trigger;
    }

    if (lhs.filter.blocking || rhs.filter.blocking) {
        return Collision::CollisionSituationKind::Blocked;
    }

    return Collision::CollisionSituationKind::Overlap;
}

void DispatchSituation(
    ICollidable *target,
    const Collision::CollisionPrimitive &selfPrimitive,
    const Collision::CollisionPrimitive &otherPrimitive,
    Collision::CollisionSituationKind kind,
    bool againstStaticWorld
) {
    if (target == nullptr) {
        return;
    }

    const auto [normal, penetration] =
        ComputeSeparation(selfPrimitive.box, otherPrimitive.box);

    Collision::CollisionSituation situation;
    situation.kind = kind;
    situation.self = target;
    situation.other = otherPrimitive.owner;
    situation.selfBox = selfPrimitive.box;
    situation.otherBox = otherPrimitive.box;
    situation.selfColliderId = selfPrimitive.colliderId;
    situation.otherColliderId = otherPrimitive.colliderId;
    situation.selfFilter = selfPrimitive.filter;
    situation.otherFilter = otherPrimitive.filter;
    situation.normal = normal;
    situation.penetration = penetration;
    situation.againstStaticWorld = againstStaticWorld;
    target->OnCollision(situation);
}

} // namespace

namespace Collision {

AxisAlignedBox CollisionSystem::BuildBox(const glm::vec2 &center, const glm::vec2 &size) {
    return {
        center,
        size
    };
}

CollisionPrimitive CollisionSystem::BuildPrimitive(
    const glm::vec2 &origin,
    const CollisionBox &box,
    ICollidable *owner
) {
    return {
        BuildBox(origin + box.offset, box.size),
        box.filter,
        owner,
        box.id,
        box.type
    };
}

CollisionPrimitive CollisionSystem::BuildStaticPrimitive(
    const AxisAlignedBox &box,
    const CollisionFilter &filter
) {
    return {
        box,
        filter,
        nullptr,
        -1,
        CollisionBoxType::Body
    };
}

std::vector<CollisionPrimitive> CollisionSystem::BuildCollisionPrimitives(
    const ICollidable &body
) {
    return BuildCollisionPrimitives(body, body.GetCollisionOrigin());
}

std::vector<CollisionPrimitive> CollisionSystem::BuildCollisionPrimitives(
    const ICollidable &body,
    const glm::vec2 &origin
) {
    std::vector<CollisionPrimitive> primitives;

    for (const CollisionBox &box : body.GetCollisionBoxes()) {
        if (!box.enabled || box.size.x <= 0.0F || box.size.y <= 0.0F) {
            continue;
        }

        primitives.push_back(BuildPrimitive(
            origin,
            box,
            const_cast<ICollidable *>(&body)
        ));
    }

    return primitives;
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

    std::vector<CollisionPrimitive> primitives = {primitive};
    MovementResult result;
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
            std::vector<CollisionPrimitive> horizontalCandidate = primitives;
            OffsetPrimitives(horizontalCandidate, {stepDelta.x, 0.0F});

            if (ArePrimitivesBlocked(
                    horizontalCandidate,
                    blockingPrimitives,
                    options,
                    *this,
                    &result.blockingOwnerX
                )) {
                result.blockedX = true;
            } else {
                result.resolvedDelta.x += stepDelta.x;
                primitives = std::move(horizontalCandidate);
            }
        }

        if (stepDelta.y != 0.0F) {
            std::vector<CollisionPrimitive> verticalCandidate = primitives;
            OffsetPrimitives(verticalCandidate, {0.0F, stepDelta.y});

            if (ArePrimitivesBlocked(
                    verticalCandidate,
                    blockingPrimitives,
                    options,
                    *this,
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

    MovementResult result;
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
            std::vector<CollisionPrimitive> horizontalCandidate = primitives;
            OffsetPrimitives(horizontalCandidate, {stepDelta.x, 0.0F});

            if (ArePrimitivesBlocked(
                    horizontalCandidate,
                    blockingPrimitives,
                    options,
                    *this,
                    &result.blockingOwnerX
                )) {
                result.blockedX = true;
            } else {
                result.resolvedDelta.x += stepDelta.x;
                primitives = std::move(horizontalCandidate);
            }
        }

        if (stepDelta.y != 0.0F) {
            std::vector<CollisionPrimitive> verticalCandidate = primitives;
            OffsetPrimitives(verticalCandidate, {0.0F, stepDelta.y});

            if (ArePrimitivesBlocked(
                    verticalCandidate,
                    blockingPrimitives,
                    options,
                    *this,
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
    std::vector<std::pair<ICollidable *, std::vector<CollisionPrimitive>>> snapshots;
    snapshots.reserve(dynamicBodies.size());

    for (ICollidable *body : dynamicBodies) {
        if (body == nullptr) {
            continue;
        }

        snapshots.emplace_back(body, BuildCollisionPrimitives(*body));
    }

    for (const auto &[body, primitives] : snapshots) {
        for (const CollisionPrimitive &selfPrimitive : primitives) {
            for (const CollisionPrimitive &staticPrimitive : staticPrimitives) {
                if (!IsInteractionEnabled(selfPrimitive, staticPrimitive, *this) ||
                    !this->IsOverlapping(selfPrimitive.box, staticPrimitive.box)) {
                    continue;
                }

                const CollisionSituationKind kind =
                    ResolveSituationKind(selfPrimitive, staticPrimitive);
                DispatchSituation(body, selfPrimitive, staticPrimitive, kind, true);
            }
        }
    }

    for (std::size_t lhsIndex = 0; lhsIndex < snapshots.size(); ++lhsIndex) {
        for (std::size_t rhsIndex = lhsIndex + 1; rhsIndex < snapshots.size(); ++rhsIndex) {
            const auto &[lhsBody, lhsPrimitives] = snapshots[lhsIndex];
            const auto &[rhsBody, rhsPrimitives] = snapshots[rhsIndex];

            for (const CollisionPrimitive &lhsPrimitive : lhsPrimitives) {
                for (const CollisionPrimitive &rhsPrimitive : rhsPrimitives) {
                    if (!IsInteractionEnabled(lhsPrimitive, rhsPrimitive, *this) ||
                        !this->IsOverlapping(lhsPrimitive.box, rhsPrimitive.box)) {
                        continue;
                    }

                    const CollisionSituationKind kind =
                        ResolveSituationKind(lhsPrimitive, rhsPrimitive);
                    DispatchSituation(lhsBody, lhsPrimitive, rhsPrimitive, kind, false);
                    DispatchSituation(rhsBody, rhsPrimitive, lhsPrimitive, kind, false);
                }
            }
        }
    }
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

    const auto buildHorizontalWallSegments =
        [](const glm::vec2 &areaCenter,
           float wallY,
           float wallWidth,
           float thickness,
           const WallOpening &opening) {
            if (wallWidth <= 0.0F || thickness <= 0.0F) {
                return std::vector<AxisAlignedBox>{};
            }

            const float openingWidth = std::clamp(opening.size, 0.0F, wallWidth);
            if (openingWidth <= 0.0F) {
                return std::vector<AxisAlignedBox>{
                    CollisionSystem::BuildBox(
                        {areaCenter.x, wallY},
                        {wallWidth, thickness}
                    )
                };
            }

            const float wallLeft = areaCenter.x - wallWidth / 2.0F;
            const float wallRight = areaCenter.x + wallWidth / 2.0F;
            const float openingHalfWidth = openingWidth / 2.0F;
            const float openingCenterX = std::clamp(
                areaCenter.x + opening.offset,
                wallLeft + openingHalfWidth,
                wallRight - openingHalfWidth
            );
            const float leftWidth =
                std::max(0.0F, openingCenterX - openingHalfWidth - wallLeft);
            const float rightWidth =
                std::max(0.0F, wallRight - openingCenterX - openingHalfWidth);
            std::vector<AxisAlignedBox> segments;

            if (leftWidth > 0.0F) {
                segments.push_back(CollisionSystem::BuildBox(
                    {wallLeft + leftWidth / 2.0F, wallY},
                    {leftWidth, thickness}
                ));
            }

            if (rightWidth > 0.0F) {
                segments.push_back(CollisionSystem::BuildBox(
                    {wallRight - rightWidth / 2.0F, wallY},
                    {rightWidth, thickness}
                ));
            }

            return segments;
        };

    const auto buildVerticalWallSegments =
        [](const glm::vec2 &areaCenter,
           float wallX,
           float wallHeight,
           float thickness,
           const WallOpening &opening) {
            if (wallHeight <= 0.0F || thickness <= 0.0F) {
                return std::vector<AxisAlignedBox>{};
            }

            const float openingHeight = std::clamp(opening.size, 0.0F, wallHeight);
            if (openingHeight <= 0.0F) {
                return std::vector<AxisAlignedBox>{
                    CollisionSystem::BuildBox(
                        {wallX, areaCenter.y},
                        {thickness, wallHeight}
                    )
                };
            }

            const float wallBottom = areaCenter.y - wallHeight / 2.0F;
            const float wallTop = areaCenter.y + wallHeight / 2.0F;
            const float openingHalfHeight = openingHeight / 2.0F;
            const float openingCenterY = std::clamp(
                areaCenter.y + opening.offset,
                wallBottom + openingHalfHeight,
                wallTop - openingHalfHeight
            );
            const float bottomHeight =
                std::max(0.0F, openingCenterY - openingHalfHeight - wallBottom);
            const float topHeight =
                std::max(0.0F, wallTop - openingCenterY - openingHalfHeight);
            std::vector<AxisAlignedBox> segments;

            if (bottomHeight > 0.0F) {
                segments.push_back(CollisionSystem::BuildBox(
                    {wallX, wallBottom + bottomHeight / 2.0F},
                    {thickness, bottomHeight}
                ));
            }

            if (topHeight > 0.0F) {
                segments.push_back(CollisionSystem::BuildBox(
                    {wallX, wallTop - topHeight / 2.0F},
                    {thickness, topHeight}
                ));
            }

            return segments;
        };

    const std::vector<AxisAlignedBox> topWalls = buildHorizontalWallSegments(
        roomCenter,
        roomCenter.y + roomHalfSize.y - halfWallThickness,
        roomSize.x,
        safeWallThickness,
        openings.top
    );
    boundaryBoxes.insert(boundaryBoxes.end(), topWalls.begin(), topWalls.end());

    const std::vector<AxisAlignedBox> bottomWalls = buildHorizontalWallSegments(
        roomCenter,
        roomCenter.y - roomHalfSize.y + halfWallThickness + 20.0F,
        roomSize.x,
        safeWallThickness,
        openings.bottom
    );
    boundaryBoxes.insert(boundaryBoxes.end(), bottomWalls.begin(), bottomWalls.end());

    const std::vector<AxisAlignedBox> leftWalls = buildVerticalWallSegments(
        roomCenter,
        roomCenter.x - roomHalfSize.x + halfWallThickness,
        roomSize.y,
        safeWallThickness,
        openings.left
    );
    boundaryBoxes.insert(boundaryBoxes.end(), leftWalls.begin(), leftWalls.end());

    const std::vector<AxisAlignedBox> rightWalls = buildVerticalWallSegments(
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
