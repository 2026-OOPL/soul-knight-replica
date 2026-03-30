#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <cmath>

#include "Component/Weapons/BadPistol.hpp"
#include "Util/Animation.hpp"
#include "Util/Logger.hpp"

#include "Component/Character/Character.hpp"

Character::Character(
    std::shared_ptr<Util::Animation> StandAnimation,
    std::shared_ptr<Util::Animation> WalkAnimation,
    std::shared_ptr<Util::Animation> DieAnimation,
    int zIndex
) : GameObject(nullptr, zIndex) {
    this->m_WalkAnimation = WalkAnimation;
    this->m_DieAnimation = DieAnimation;
    this->m_StandAnimation = StandAnimation;

    this->m_Weapon = std::make_shared<BadPistol>();

    this->AddChild(m_Weapon);
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

    this->m_Weapon = std::make_shared<BadPistol>();

    this->AddChild(m_Weapon);
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

Util::Transform Character::GetObjectTransform() const {
    return this->m_Transform;
}

std::shared_ptr<Weapon> Character::GetWeapon() {
    return m_Weapon;
}

void Character::SetWeapon(std::shared_ptr<Weapon> weapon) {
    this->m_Weapon = weapon;
}

glm::vec2 Character::GetMoveIntent() const {
    return glm::vec2(0, 0);
}

void Character::Update() {
    const glm::vec2 moveIntent = this->GetMoveIntent();

    // Set weapon position
    if (this->m_Weapon != nullptr && moveIntent != glm::vec2(0, 0)) {
        m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
        m_Weapon->SetFacingDirection(moveIntent);
    }

    this->SetSpriteTypeByMoveIntent(moveIntent);

    if (moveIntent != glm::vec2(0, 0)) {
        this->SetLookDirectionByMoveIntent(moveIntent);
        m_LastMomentum = moveIntent;
    }

}