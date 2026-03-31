#ifndef I_COLLIDABLE_HPP
#define I_COLLIDABLE_HPP

#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionTypes.hpp"

class ICollidable {
public:
    virtual ~ICollidable() = default;

    virtual glm::vec2 GetCollisionOrigin() const = 0;
    virtual const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const = 0;
    virtual void OnCollision(const Collision::CollisionSituation &situation) {
        (void)situation;
    }
};

#endif
