#ifndef TESTBULLET_HPP
#define TESTBULLET_HPP

#include "Component/Bullet.hpp"
#include <vector>

namespace {
    const std::vector<std::string> BULLET_SPRITE = {
        RESOURCE_DIR"/Bullet/TestBullet.png"
    };
}

class TestBullet : public Bullet {
public:
    TestBullet(
        glm::vec2 cooridinate,
        glm::vec2 momentum
    ) : Bullet(
        BULLET_SPRITE,
        cooridinate,
        momentum,
        1
    ) {
        this->SetAbsoluteScale(glm::vec2(0.05F, 0.05F));
        this->m_Transform.scale = this->m_AbsoluteTransform.scale;
        this->SetColliderSize(this->GetScaledSize());
    };

};

#endif
