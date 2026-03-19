#ifndef I_COLLIDABLE_HPP
#define I_COLLIDABLE_HPP

#include "Util/Transform.hpp"
#include <glm/glm.hpp>/*為了使用到 glm::vec2 裡的成員型別*/
#include <memory>
#include <vector>

// TODO: 實作圓形檢測

class Collider {
    Collider(Util::Transform transform) {
        this->m_Transform = transform;
    };

    Util::Transform m_Transform;

    virtual bool isCollideWith(std::shared_ptr<Collider> other) = 0;
};

class CircleCollider : public Collider {
public:

protected:
    
};

class RectangleCollider : public Collider {
public:

    
protected:
    
};


class ICollidable {
public:
    virtual std::vector<std::shared_ptr<Collider>> GetCollideBox() = 0;

    virtual bool WillCollide() = 0;

    // struct RectCollider/*定義碰撞箱*/
    // {
    //     glm::vec2 center;/*矩形中心點*/
    //     glm::vec2 size;/*矩形寬高*/
    // };

    // virtual ~ICollidable() = default;

    // /*如果繼承ICollidable就必須實作GetCollider()還有CanBlockMovement()*/
    // virtual RectCollider GetCollider() const = 0;//你要回傳自己的碰撞矩形
    // virtual bool CanBlockMovement() const = 0;//你要回答這這物件會不會擋路
    
    // /*增加可讀性使用此函式把
    // ICollidable::IsOverlapping(a.GetCollider(), b.GetCollider())改成
    // a.IsCollidingWith(b)*/
    // bool IsCollidingWith(const ICollidable &other) const {
    //     return IsOverlapping(GetCollider(), other.GetCollider());
    // }
    // /*碰撞邏輯實作*/
    // static bool IsOverlapping(const RectCollider &lhs,
    //                           const RectCollider &rhs) {
                                
    //     const float lhsLeft = lhs.center.x - lhs.size.x / 2.0F;//左邊界位置 = 中心 x - 半個寬
    //     const float lhsRight = lhs.center.x + lhs.size.x / 2.0F;//右邊界位置 = 中心 x + 半個寬
    //     const float lhsTop = lhs.center.y + lhs.size.y / 2.0F;//上邊界位置 = 中心 y + 半個高
    //     const float lhsBottom = lhs.center.y - lhs.size.y / 2.0F;//下邊界位置 = 中心 y - 半個高

    //     const float rhsLeft = rhs.center.x - rhs.size.x / 2.0F;
    //     const float rhsRight = rhs.center.x + rhs.size.x / 2.0F;
    //     const float rhsTop = rhs.center.y + rhs.size.y / 2.0F;
    //     const float rhsBottom = rhs.center.y - rhs.size.y / 2.0F;

    //     /*有重疊就回傳ture*/
    //     return !(lhsRight <= rhsLeft || lhsLeft >= rhsRight ||
    //              lhsTop <= rhsBottom || lhsBottom >= rhsTop);
    // }


};

#endif
