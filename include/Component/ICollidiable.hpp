#ifndef COLLIDABLE_HPP
#define COLLIDABLE_HPP

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

class ICollidable {
public:
    virtual ~ICollidable() = default;

    virtual glm::vec2 GetColliderSize() const = 0;
    virtual void SetColliderSize(const glm::vec2 &colliderSize) = 0;

    virtual glm::vec2 GetColliderCooridinate() const = 0;
};

#endif // COLLIDABLE_HPP