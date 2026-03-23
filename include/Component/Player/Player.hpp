#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include <functional>

#include <glm/fwd.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/ICollidable.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include "Util/Animation.hpp"

class Player : public Character, public ICollidable, public IMapObject, public IStateful {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const Collision::AxisAlignedBox &,
        const glm::vec2 &
    )>;

    Player() : Character(
        std::make_shared<Util::Animation>(
            std::vector<std::string>{
                RESOURCE_DIR"/Character/Test/test_stand.png"
            },
            true,
            1
        )
    ) {
        this->m_Cooridinate = {0.0F, 0.0F};
    }

    void Update() override;

    glm::vec2 GetObjectSize() override;
    glm::vec2 GetCooridinate() override;
    Util::Transform GetTransform() override;

    bool WillCollide() override;
    std::vector<std::shared_ptr<Collider>> GetCollideBox() override;

    glm::vec2 GetColliderSize();
    void SetColliderSize(const glm::vec2 &colliderSize);
    glm::vec2 GetPosition() const;
    void SetPosition(const glm::vec2 &position);
    void SetCollisionResolver(CollisionResolver collisionResolver);

    glm::vec2 GetMoveIntent() const;

    glm::vec2 m_Cooridinate;

private:
    glm::vec2 m_ColliderSize = {48.0F, 48.0F};
    CollisionResolver m_CollisionResolver = nullptr;
};

#endif
