#include <cstdlib>
#include <utility>
#include <vector>

#include "Component/Collision/CollisionDispatcher.hpp"

#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Collision/ICollidable.hpp"

namespace {

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

void CollisionDispatcher::Dispatch(
    const CollisionSystem &collisionSystem,
    const std::vector<CollisionPrimitive> &staticPrimitives,
    const std::vector<ICollidable *> &dynamicBodies
) {
    std::vector<std::pair<ICollidable *, std::vector<CollisionPrimitive>>> snapshots;
    snapshots.reserve(dynamicBodies.size());

    for (ICollidable *body : dynamicBodies) {
        if (body == nullptr) {
            continue;
        }

        snapshots.emplace_back(body, CollisionSystem::BuildCollisionPrimitives(*body));
    }

    for (const auto &[body, primitives] : snapshots) {
        for (const CollisionPrimitive &selfPrimitive : primitives) {
            for (const CollisionPrimitive &staticPrimitive : staticPrimitives) {
                if (!IsInteractionEnabled(selfPrimitive, staticPrimitive, collisionSystem) ||
                    !collisionSystem.IsOverlapping(selfPrimitive.box, staticPrimitive.box)) {
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
                    if (!IsInteractionEnabled(lhsPrimitive, rhsPrimitive, collisionSystem) ||
                        !collisionSystem.IsOverlapping(lhsPrimitive.box, rhsPrimitive.box)) {
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

} // namespace Collision
