#ifndef I_BLOCKING_PRIMITIVE_SOURCE_HPP
#define I_BLOCKING_PRIMITIVE_SOURCE_HPP

#include <vector>

#include "Component/Collision/CollisionSystem.hpp"

namespace Collision {

inline CollisionFilter MakeWorldBlockingFilter() {
    CollisionFilter filter;
    filter.layer = CollisionLayer::World;
    filter.mask = kAllCollisionLayers;
    filter.blocking = true;
    return filter;
}

inline std::vector<CollisionPrimitive> BuildStaticWorldPrimitives(
    const std::vector<AxisAlignedBox> &boxes
) {
    std::vector<CollisionPrimitive> primitives;
    const CollisionFilter filter = MakeWorldBlockingFilter();

    primitives.reserve(boxes.size());
    for (const auto &box : boxes) {
        primitives.push_back(CollisionSystem::BuildStaticPrimitive(box, filter));
    }

    return primitives;
}

} // namespace Collision

class IBlockingPrimitiveSource {
public:
    virtual ~IBlockingPrimitiveSource() = default;

    virtual std::vector<Collision::CollisionPrimitive> CollectBlockingPrimitives(
        const Collision::AxisAlignedBox *ignoreOverlapBox = nullptr
    ) const = 0;
};

#endif
