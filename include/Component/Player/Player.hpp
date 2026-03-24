#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include <functional>
#include <memory>
#include <vector>

#include <glm/fwd.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include "Util/Animation.hpp"

class Player : public Character, public IMapObject, public IStateful {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const Collision::AxisAlignedBox &,
        const glm::vec2 &
    )>;

    Player();

    void Update() override;

    glm::vec2 GetObjectSize() override;
    glm::vec2 GetCooridinate() override;
    Util::Transform GetTransform() override;

    glm::vec2 GetColliderSize();
    void SetColliderSize(const glm::vec2 &colliderSize);
    glm::vec2 GetPosition() const;
    void SetPosition(const glm::vec2 &position);
    Collision::AxisAlignedBox GetCollisionBox() const;
    Collision::AxisAlignedBox GetCollisionBoxAt(const glm::vec2 &coordinate) const;
    void SetCollisionResolver(CollisionResolver collisionResolver);

    glm::vec2 GetMoveIntent() const;

    glm::vec2 m_Cooridinate;

private:
    glm::vec2 m_ColliderSize = {24.0F, 24.0F};
    glm::vec2 m_PendingMoveDelta = {0.0F, 0.0F};
    CollisionResolver m_CollisionResolver = nullptr;
};

#endif
