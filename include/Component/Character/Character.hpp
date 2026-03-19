#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include <memory>

class Character : public Util::GameObject {
public:
    Character(
        std::shared_ptr<Util::Animation> WalkAnimation
    ) : GameObject(WalkAnimation, 5) {
        this->m_WalkAnimation = WalkAnimation;
    };



protected:
    float m_PlayerSpeed = 0.35F;
    std::shared_ptr<Util::Animation> m_WalkAnimation;
};

#endif