#ifndef COLLISION_TYPES_HPP
#define COLLISION_TYPES_HPP

#include <cstdint>

#include <glm/vec2.hpp>

class ICollidable;

namespace Collision {

enum class CollisionLayer : std::uint32_t {
    None = 0U,
    World = 1U << 0U,
    Player = 1U << 1U,
    Enemy = 1U << 2U,
    PlayerProjectile = 1U << 3U,
    EnemyProjectile = 1U << 4U,
    Prop = 1U << 5U,
    Trigger = 1U << 6U
};

using CollisionMask = std::uint32_t;

constexpr CollisionMask ToMask(CollisionLayer layer) {
    return static_cast<CollisionMask>(layer);
}

constexpr CollisionMask kAllCollisionLayers = 0xFFFFFFFFU;

inline bool MatchesMask(CollisionLayer layer, CollisionMask mask) {
    return (mask & ToMask(layer)) != 0U;
}

struct AxisAlignedBox {
    glm::vec2 center = {0.0F, 0.0F};
    glm::vec2 size = {0.0F, 0.0F};
};

enum class CollisionBoxType {
    Body,
    Hitbox,
    Hurtbox,
    Trigger
};

struct CollisionFilter {
    CollisionLayer layer = CollisionLayer::World;
    CollisionMask mask = kAllCollisionLayers;
    bool blocking = false;
    bool trigger = false;
};

struct CollisionBox {
    int id = -1;
    CollisionBoxType type = CollisionBoxType::Body;
    glm::vec2 offset = {0.0F, 0.0F};
    glm::vec2 size = {0.0F, 0.0F};
    CollisionFilter filter;
    bool enabled = true;
};

enum class CollisionSituationKind {
    Blocked,
    Overlap,
    Trigger
};

struct CollisionSituation {
    virtual ~CollisionSituation() = default;

    CollisionSituationKind kind = CollisionSituationKind::Overlap;
    ICollidable *self = nullptr;
    ICollidable *other = nullptr;
    AxisAlignedBox selfBox;
    AxisAlignedBox otherBox;
    int selfColliderId = -1;
    int otherColliderId = -1;
    CollisionFilter selfFilter;
    CollisionFilter otherFilter;
    glm::vec2 normal = {0.0F, 0.0F};
    glm::vec2 penetration = {0.0F, 0.0F};
    bool againstStaticWorld = false;
};

struct CollisionPrimitive {
    AxisAlignedBox box;
    CollisionFilter filter;
    ICollidable *owner = nullptr;
    int colliderId = -1;
    CollisionBoxType type = CollisionBoxType::Body;
};

struct CollisionQueryOptions {
    CollisionMask blockerMask = kAllCollisionLayers;
    const ICollidable *ignoreOwner = nullptr;
};

struct MovementResult {
    glm::vec2 resolvedDelta = {0.0F, 0.0F};
    bool blockedX = false;
    bool blockedY = false;
    ICollidable *blockingOwnerX = nullptr;
    ICollidable *blockingOwnerY = nullptr;
};

} // namespace Collision

#endif
