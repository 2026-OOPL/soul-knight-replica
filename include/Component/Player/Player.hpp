#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include "Component/Character/Character.hpp"
#include "Component/ICollidable.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include "Util/Animation.hpp"
#include <glm/fwd.hpp>

class Player : public Character, public ICollidable, public IMapObject, public IStateful {
public:
    Player() : Character(
        std::make_shared<Util::Animation>(
            std::vector<std::string>{
                RESOURCE_DIR"/Character/Character.png"
            },
            true,
            1
        )
    ) {
        m_Cooridinate = {0, 0};
    }

    void Update() override;

    // Provide data for IMapObject
    glm::vec2 GetObjectSize() override;
    glm::vec2 GetCooridinate() override;
    Util::Transform GetTransform() override;
    
    // Provide data for ICollidable
    bool WillCollide() override;
    std::vector<std::shared_ptr<Collider>> GetCollideBox() override;

    glm::vec2 GetColliderSize(); // 回傳角色碰撞箱大小
    void SetColliderSize(const glm::vec2 &colliderSize); // 設定角色碰撞箱大小
    glm::vec2 GetPosition() const; // 回傳角色中心位置
    void SetPosition(const glm::vec2 &position);

    glm::vec2 GetMoveIntent() const; 

    // The absolute position on the map
    glm::vec2 m_Cooridinate;

private:
    //玩家的碰撞盒尺寸
    glm::vec2 m_ColliderSize = {48.0F, 48.0F};
};

#endif
