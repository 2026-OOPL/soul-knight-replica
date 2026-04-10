#ifndef COLLISION_QUERY_SERVICE_HPP
#define COLLISION_QUERY_SERVICE_HPP

#include <functional>
#include <utility>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionSystem.hpp"

class ICollidable;

class CollisionQueryService {
public:
    using BlockingPrimitiveProvider = std::function<std::vector<Collision::CollisionPrimitive>(
        const ICollidable *ignoreBody
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
            this->GetBlockingPrimitives(&body),
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
            this->GetBlockingPrimitives(&body),
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
            this->GetBlockingPrimitives(&body),
            this->BuildOptions(body)
        );
    }

private:
    Collision::CollisionQueryOptions BuildOptions(const ICollidable &body) const {
        Collision::CollisionQueryOptions options;
        options.ignoreOwner = &body;
        options.blockerMask = this->m_DefaultBlockerMask;
        return options;
    }

    std::vector<Collision::CollisionPrimitive> GetBlockingPrimitives(
        const ICollidable *ignoreBody
    ) const {
        if (!this->m_BlockingPrimitiveProvider) {
            return {};
        }

        return this->m_BlockingPrimitiveProvider(ignoreBody);
    }

    Collision::CollisionSystem *m_CollisionSystem = nullptr;
    BlockingPrimitiveProvider m_BlockingPrimitiveProvider;
    Collision::CollisionMask m_DefaultBlockerMask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop);
};

#endif
