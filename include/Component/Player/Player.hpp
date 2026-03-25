#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include <vector>
#include <memory>
#include <functional>

#include <glm/vec2.hpp>
#include "Util/Animation.hpp"
#include "Util/Transform.hpp"

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"

class Player : public Character, public IMapObject, public IStateful {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const Collision::AxisAlignedBox &,
        const glm::vec2 &
    )>;

    Player();

    void Update() override;

    // These are the getter of the m_Transform in Util::GameObject
    glm::vec2 GetAbsoluteScale() override;
    Util::Transform GetObjectTransform() override;

    // These are the getter of the m_AbsoluteTransform in MapSystem
    glm::vec2 GetAbsolutePosition() const;
    Util::Transform GetAbsoluteTransform() override;

    glm::vec2 GetColliderSize();
    void SetColliderSize(const glm::vec2 &colliderSize);
    void SetPosition(const glm::vec2 &position);
    Collision::AxisAlignedBox GetCollisionBox() const;
    Collision::AxisAlignedBox GetCollisionBoxAt(const glm::vec2 &coordinate) const;
    void SetCollisionResolver(CollisionResolver collisionResolver);

    glm::vec2 GetMoveIntent() const;

    // The absolute position on the map
    Util::Transform m_AbsoluteTransform;
private:
    glm::vec2 m_ColliderSize = {24.0F, 24.0F};
    glm::vec2 m_PendingMoveDelta = {0.0F, 0.0F};
    CollisionResolver m_CollisionResolver = nullptr;
};

#endif
