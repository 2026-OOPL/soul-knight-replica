#ifndef I_COLLIDABLE_HPP
#define I_COLLIDABLE_HPP

#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionTypes.hpp"

class ICollidable {
public:
    virtual ~ICollidable() = default;

    virtual glm::vec2 GetCollisionOrigin() const = 0; //回傳碰撞原點
    virtual const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const = 0;//回傳這個物件有哪些碰撞盒，大小、偏移、layer、mask、是否 blocking
    virtual void OnCollision(const Collision::CollisionSituation &situation) {//當碰撞系統偵測到碰撞時，通知你要怎麼反應
        (void)situation;
    }
};

#endif
