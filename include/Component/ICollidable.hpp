#include "Util/Transform.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class HitBox {
public:
    Util::Transform m_Transform;
    std::vector<std::shared_ptr<HitBox>> m_HitBox;

    HitBox(std::vector<std::shared_ptr<HitBox>> &hitbox, Util::Transform transform) {
        this->m_Transform = transform;
        this->m_HitBox = hitbox;
    };

    Util::Transform GetTransformWithOffset(Util::Transform others) {
        Util::Transform result;

        result.translation = {
            m_Transform.translation.x + others.translation.x,
            m_Transform.translation.y + others.translation.y
        };

        result.scale = 
            m_Transform.scale * others.scale;
        
        result.rotation = 
            m_Transform.rotation + m_Transform.rotation;

        return result;
    }
};

class RectHitBox : public HitBox {
public:
    RectHitBox(
        float width,
        float height,
        Util::Transform transform,
        std::vector<std::shared_ptr<HitBox>> &hitBoxes
    ) : HitBox(hitBoxes, transform) {
        this->m_Width = width;
        this->m_Height = height;
    }


private:
    float m_Width;
    float m_Height;
};

class CircHitBox : public HitBox {
public:
    int m_Raduis;

    CircHitBox(
        int raduis,
        Util::Transform transform,
        std::vector<std::shared_ptr<HitBox>> &hitBoxes
    ) : HitBox(hitBoxes, transform) {
        this->m_Raduis = raduis;
    }
};

class ICollidable {
public:
    bool isCollideWith(std::shared_ptr<ICollidable> other) {
        this->GetTransform();
        other->GetTransform();

        return false;
    }

    virtual HitBox GetHitbox() = 0;
    virtual Util::Transform GetTransform() = 0;

private:

    static bool CalculateCollide(CircHitBox circ, RectHitBox rect) {
        return CalculateCollide(rect, circ);
    }

    static bool CalculateCollide(RectHitBox rect, CircHitBox circ) {
        
        return false;
    }



};