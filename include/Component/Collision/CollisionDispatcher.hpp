#ifndef COLLISION_DISPATCHER_HPP
#define COLLISION_DISPATCHER_HPP

#include <vector>

#include "Component/Collision/CollisionTypes.hpp"

class ICollidable;

namespace Collision {

class CollisionSystem;

class CollisionDispatcher {
public:
    static void Dispatch(
        const CollisionSystem &collisionSystem,
        const std::vector<CollisionPrimitive> &staticPrimitives,
        const std::vector<ICollidable *> &dynamicBodies
    );
};

} // namespace Collision

#endif
