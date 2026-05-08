#include "Component/Collision/CollisionPrimitiveBuilder.hpp"

#include "Component/Collision/ICollidable.hpp"

namespace Collision {

AxisAlignedBox CollisionPrimitiveBuilder::BuildBox(
    const glm::vec2 &center,
    const glm::vec2 &size
) {
    return {
        center,
        size
    };
}

CollisionPrimitive CollisionPrimitiveBuilder::BuildPrimitive(
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

CollisionPrimitive CollisionPrimitiveBuilder::BuildStaticPrimitive(
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

std::vector<CollisionPrimitive> CollisionPrimitiveBuilder::BuildCollisionPrimitives(
    const ICollidable &body
) {
    return BuildCollisionPrimitives(body, body.GetCollisionOrigin());
}

std::vector<CollisionPrimitive> CollisionPrimitiveBuilder::BuildCollisionPrimitives(
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

} // namespace Collision
