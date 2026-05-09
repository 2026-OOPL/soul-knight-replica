#ifndef COLLISION_QUERY_SERVICE_HPP
#define COLLISION_QUERY_SERVICE_HPP

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionSystem.hpp"

class ICollidable;

class CollisionQueryService {
public:
    struct BlockingPrimitiveQuery {
        const ICollidable *ignoreBody = nullptr;
        Collision::AxisAlignedBox queryBox;
    };

    using BlockingPrimitiveProvider = std::function<std::vector<Collision::CollisionPrimitive>(
        const BlockingPrimitiveQuery &query
    )>;

    explicit CollisionQueryService(Collision::CollisionSystem *collisionSystem = nullptr)
        : m_CollisionSystem(collisionSystem) {
    }

    void SetCollisionSystem(Collision::CollisionSystem *collisionSystem) {
        this->m_CollisionSystem = collisionSystem;
    }

    void SetBlockingPrimitiveProvider(BlockingPrimitiveProvider blockingPrimitiveProvider) {
        this->m_BlockingPrimitiveProvider = std::move(blockingPrimitiveProvider);
    }

    void SetDefaultBlockerMask(Collision::CollisionMask blockerMask) {
        this->m_DefaultBlockerMask = blockerMask;
    }

    Collision::MovementResult ResolveMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    ) const {
        if (this->m_CollisionSystem == nullptr) {
            return {};
        }

        return this->m_CollisionSystem->ResolveMovement(
            body,
            intendedDelta,
            this->GetBlockingPrimitives(this->BuildMovementQuery(body, intendedDelta)),
            this->BuildOptions(body)
        );
    }

    Collision::MovementResult PredictMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    ) const {
        if (this->m_CollisionSystem == nullptr) {
            return {};
        }

        return this->m_CollisionSystem->PredictMovement(
            body,
            intendedDelta,
            this->GetBlockingPrimitives(this->BuildMovementQuery(body, intendedDelta)),
            this->BuildOptions(body)
        );
    }

    bool CanOccupy(
        const ICollidable &body,
        const glm::vec2 &targetOrigin
    ) const {
        if (this->m_CollisionSystem == nullptr) {
            return true;
        }

        return this->m_CollisionSystem->CanOccupy(
            body,
            targetOrigin,
            this->GetBlockingPrimitives(this->BuildOccupancyQuery(body, targetOrigin)),
            this->BuildOptions(body)
        );
    }

private:
    static Collision::AxisAlignedBox MoveBox(
        Collision::AxisAlignedBox box,
        const glm::vec2 &delta
    ) {
        box.center += delta;
        return box;
    }

    static Collision::AxisAlignedBox MergeBoxes(
        const Collision::AxisAlignedBox &lhs,
        const Collision::AxisAlignedBox &rhs
    ) {
        const glm::vec2 lhsHalfSize = lhs.size / 2.0F;
        const glm::vec2 rhsHalfSize = rhs.size / 2.0F;

        const float left = std::min(
            lhs.center.x - lhsHalfSize.x,
            rhs.center.x - rhsHalfSize.x
        );
        const float right = std::max(
            lhs.center.x + lhsHalfSize.x,
            rhs.center.x + rhsHalfSize.x
        );
        const float bottom = std::min(
            lhs.center.y - lhsHalfSize.y,
            rhs.center.y - rhsHalfSize.y
        );
        const float top = std::max(
            lhs.center.y + lhsHalfSize.y,
            rhs.center.y + rhsHalfSize.y
        );

        return Collision::CollisionSystem::BuildBox(
            {(left + right) / 2.0F, (bottom + top) / 2.0F},
            {right - left, top - bottom}
        );
    }

    static Collision::AxisAlignedBox InflateBox(
        Collision::AxisAlignedBox box,
        float padding
    ) {
        const float safePadding = std::max(0.0F, padding);
        box.size += glm::vec2(safePadding * 2.0F);
        return box;
    }

    static Collision::AxisAlignedBox BuildPrimitiveUnion(
        const std::vector<Collision::CollisionPrimitive> &primitives
    ) {
        if (primitives.empty()) {
            return {};
        }

        Collision::AxisAlignedBox result = primitives.front().box;
        for (std::size_t index = 1; index < primitives.size(); ++index) {
            result = MergeBoxes(result, primitives[index].box);
        }
        return result;
    }

    BlockingPrimitiveQuery BuildMovementQuery(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    ) const {
        const std::vector<Collision::CollisionPrimitive> primitives =
            Collision::CollisionSystem::BuildCollisionPrimitives(body);

        Collision::AxisAlignedBox queryBox = BuildPrimitiveUnion(primitives);
        if (primitives.empty()) {
            queryBox = Collision::CollisionSystem::BuildBox(
                body.GetCollisionOrigin() + intendedDelta / 2.0F,
                {std::abs(intendedDelta.x), std::abs(intendedDelta.y)}
            );
        } else {
            for (const Collision::CollisionPrimitive &primitive : primitives) {
                queryBox = MergeBoxes(
                    queryBox,
                    MoveBox(primitive.box, intendedDelta)
                );
            }
        }

        return {
            &body,
            InflateBox(queryBox, kQueryPadding)
        };
    }

    BlockingPrimitiveQuery BuildOccupancyQuery(
        const ICollidable &body,
        const glm::vec2 &targetOrigin
    ) const {
        const std::vector<Collision::CollisionPrimitive> primitives =
            Collision::CollisionSystem::BuildCollisionPrimitives(body, targetOrigin);

        Collision::AxisAlignedBox queryBox = BuildPrimitiveUnion(primitives);
        if (primitives.empty()) {
            queryBox = Collision::CollisionSystem::BuildBox(targetOrigin, {0.0F, 0.0F});
        }

        return {
            &body,
            InflateBox(queryBox, kQueryPadding)
        };
    }

    Collision::CollisionQueryOptions BuildOptions(const ICollidable &body) const {
        Collision::CollisionQueryOptions options;
        options.ignoreOwner = &body;
        options.blockerMask = this->m_DefaultBlockerMask;
        return options;
    }

    std::vector<Collision::CollisionPrimitive> GetBlockingPrimitives(
        const BlockingPrimitiveQuery &query
    ) const {
        if (!this->m_BlockingPrimitiveProvider) {
            return {};
        }

        return this->m_BlockingPrimitiveProvider(query);
    }

    static constexpr float kQueryPadding = 16.0F;

    Collision::CollisionSystem *m_CollisionSystem = nullptr;
    BlockingPrimitiveProvider m_BlockingPrimitiveProvider;
    Collision::CollisionMask m_DefaultBlockerMask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop);
};

#endif
