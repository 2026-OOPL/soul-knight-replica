#ifndef COLLISION_PRIMITIVE_BUILDER_HPP
#define COLLISION_PRIMITIVE_BUILDER_HPP

#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionTypes.hpp"

class ICollidable;

namespace Collision {

class CollisionPrimitiveBuilder {
public:
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
};

} // namespace Collision

#endif
