#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include <vector>
#include <functional>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"

class Player : public Character {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const Collision::AxisAlignedBox &,
        const glm::vec2 &
    )>;

    Player(
        const std::vector<std::string>& StandSprite,
        const std::vector<std::string>& WalkSprite,
        const std::vector<std::string>& DieSprite
    );

    void Update() override;

    glm::vec2 GetColliderSize();
    void SetColliderSize(const glm::vec2 &colliderSize);
    void SetPosition(const glm::vec2 &position);
    Collision::AxisAlignedBox GetCollisionBox() const;
    Collision::AxisAlignedBox GetCollisionBoxAt(const glm::vec2 &coordinate) const;
    void SetCollisionResolver(CollisionResolver collisionResolver);

    glm::vec2 GetMoveIntent() const;

private:
    glm::vec2 m_ColliderSize = {24.0F, 24.0F};
    glm::vec2 m_PendingMoveDelta = {0.0F, 0.0F};

    CollisionResolver m_CollisionResolver = nullptr;
};

#endif
