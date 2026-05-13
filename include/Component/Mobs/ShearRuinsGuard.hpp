#ifndef SHEAR_RUINS_GUARD_HPP
#define SHEAR_RUINS_GUARD_HPP

#include <memory>
#include <vector>

#include <glm/geometric.hpp>

#include "Component/AI/MeleeAI.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Component/Weapons/Shear.hpp"

namespace {
    const std::vector<std::string> SHEAR_RUINS_GUARD_WALK_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_1.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_2.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_3.png"
    };

    const std::vector<std::string> SHEAR_RUINS_GUARD_STAND_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_1.png"
    };

    const std::vector<std::string> SHEAR_RUINS_GUARD_DIE_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_die.png"
    };
}

class ShearRuinsGuard : public Mob {
public:
    ShearRuinsGuard(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Mob(
            SHEAR_RUINS_GUARD_STAND_SPRITE,
            SHEAR_RUINS_GUARD_WALK_SPRITE,
            SHEAR_RUINS_GUARD_DIE_SPRITE,
            tracePlayer,
            collisionSystem
        ) {
        this->SetAttackAnimation(std::make_shared<Util::Animation>(
            SHEAR_RUINS_GUARD_STAND_SPRITE,
            false,
            30,
            false,
            0,
            false
        ));
        this->SetMaxHealth(5);
        this->SetCurrentHealth(this->GetMaxHealth());
        this->SetWeapon(std::make_shared<Shear>());

        MeleeAIConfig aiConfig;
        aiConfig.attackRange = 70.0F;
        aiConfig.attackReleaseRange = 92.0F;
        this->m_AI = std::make_shared<MeleeAI>(
            this,
            tracePlayer.lock(),
            collisionSystem,
            aiConfig
        );
    }

protected:
    void UpdateWeaponPresentation() override {
        if (this->m_Weapon == nullptr) {
            return;
        }

        const bool isIdle =
            this->m_AI == nullptr ||
            (!this->m_AI->GetAttackTrigger() &&
             glm::length(this->GetMoveIntent()) <= 0.0001F);

        if (!isIdle) {
            this->m_WeaponSocketOffset = {0.0F, 0.0F};
            Mob::UpdateWeaponPresentation();
            return;
        }

        this->m_WeaponSocketOffset = {-10.0F, -20.0F};
        this->m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
        this->m_Weapon->SetSocketOffset(this->m_WeaponSocketOffset);
        this->m_Weapon->SetFacingDirection({0.0F, 1.0F});
    }
};

#endif
