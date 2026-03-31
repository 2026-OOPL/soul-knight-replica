#ifndef BULLET_HPP
#define BULLET_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <glm/fwd.hpp>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"

#include "Common/MapObject.hpp"
#include "Component/Collision/ICollidable.hpp"
#include "Component/IStateful.hpp"
#include "Component/Weapon.hpp"

class Bullet : public MapObject,
               public IStateful,
               public Util::GameObject,
               public ICollidable {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const ICollidable &,
        const glm::vec2 &
    )>;

    Bullet(
        std::shared_ptr<Util::Animation> animation,
        glm::vec2 cooridinate,
        glm::vec2 momentum,
        int zIndex
    );

    Bullet(
        const std::vector<std::string>& sprite,
        glm::vec2 cooridinate,
        glm::vec2 momentum,
        int zIndex
    );

    // Override for MapObject
    Util::Transform GetObjectTransform() const override; 

    void Update() override;
    glm::vec2 GetCollisionOrigin() const override;
    const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const override;
    void OnCollision(const Collision::CollisionSituation &situation) override;

    glm::vec2 GetColliderSize() const;
    void SetColliderSize(const glm::vec2 &colliderSize);
    void SetCollisionFilter(const Collision::CollisionFilter &filter);
    void SetCollisionResolver(CollisionResolver collisionResolver);
    bool IsDestroyRequested() const;
    void RequestDestroy();

protected:
    glm::vec2 m_Momentum;

    std::shared_ptr<Util::Animation> m_Animation;
    std::vector<Collision::CollisionBox> m_CollisionBoxes;
    CollisionResolver m_CollisionResolver = nullptr;
    bool m_DestroyRequested = false;
};

#endif
