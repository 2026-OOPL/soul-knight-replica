#include <memory>

#include "Component/Character/Character.hpp"
#include "Util/Animation.hpp"

Character::Character(
    std::shared_ptr<Util::Animation> StandAnimation,
    std::shared_ptr<Util::Animation> WalkAnimation,
    std::shared_ptr<Util::Animation> DieAnimation,
    int zIndex
) : GameObject(nullptr, zIndex) {
    this->m_WalkAnimation = WalkAnimation;
    this->m_DieAnimation = DieAnimation;
    this->m_StandAnimation = StandAnimation;

    this->SetDrawable(this->m_StandAnimation);
};

Character::Character(
    const std::vector<std::string>& StandSprite,
    const std::vector<std::string>& WalkSprite,
    const std::vector<std::string>& DieSprite,
    int zIndex
) : GameObject(nullptr, zIndex) {
    this->m_WalkAnimation = std::make_shared<Util::Animation>(
        WalkSprite, true, 20, true, 0
    );
    
    this->m_DieAnimation = std::make_shared<Util::Animation>(
        DieSprite, true, 20, true, 0
    );

    this->m_StandAnimation = std::make_shared<Util::Animation>(
        StandSprite, true, 20, true, 0
    );

    this->SetDrawable(this->m_StandAnimation);
};

void Character::SetLookDirectionByMoveIntent(glm::vec2 moveIntent) {
    if (moveIntent.x > 0 && this->m_AbsoluteTransform.scale.x < 0) {
        this->m_AbsoluteTransform.scale.x *= -1;
        return;
    }

    if (moveIntent.x < 0 && this->m_AbsoluteTransform.scale.x > 0) {
        this->m_AbsoluteTransform.scale.x *= -1;
    }
}

void Character::SetSpriteTypeByMoveIntent(glm::vec2 moveIntent) {
    if (moveIntent == glm::vec2(0.0F, 0.0F)) {
        this->SetDrawable(this->m_StandAnimation);
        return;
    }

    this->SetDrawable(this->m_WalkAnimation);
}

glm::vec2 Character::GetAbsoluteScale() {
    return this->m_Drawable->GetSize() * this->m_AbsoluteTransform.scale;
}

Util::Transform Character::GetAbsoluteTransform() {
    return this->m_AbsoluteTransform;
}

Util::Transform Character::GetObjectTransform() {
    return this->m_Transform;
}

glm::vec2 Character::GetAbsolutePosition() const {
    return this->m_AbsoluteTransform.translation;
}

void Character::SetAbsoluteScale(glm::vec2 scale) {
    this->m_AbsoluteTransform.scale = scale;
}

void Character::SetAbsoluteTransform(glm::vec2 transform) {
    this->m_AbsoluteTransform.translation = transform;
}

void Character::SetAbsoluteRotation(float degree) {
    this->m_AbsoluteTransform.rotation = degree;
}