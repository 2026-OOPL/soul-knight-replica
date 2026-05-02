#ifndef BOW_RUINS_GUARD_HPP
#define BOW_RUINS_GUARD_HPP

#include <memory>
#include <vector>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Component/Weapons/Bow.hpp"

namespace {
    const std::vector<std::string> BOW_RUINS_GUARD_WALK_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_1.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_2.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_3.png"
    };

    const std::vector<std::string> BOW_RUINS_GUARD_STAND_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_1.png"
    };

    const std::vector<std::string> BOW_RUINS_GUARD_DIE_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_die.png"
    };
}

class BowRuinsGuard : public Mob {
public:
    BowRuinsGuard(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Mob(
            BOW_RUINS_GUARD_STAND_SPRITE,
            BOW_RUINS_GUARD_WALK_SPRITE,
            BOW_RUINS_GUARD_DIE_SPRITE,
            tracePlayer,
            collisionSystem
        ) {
        this->SetAttackAnimation(std::make_shared<Util::Animation>(
            BOW_RUINS_GUARD_STAND_SPRITE,
            false,
            30,
            false,
            0,
            false
        ));
        this->SetMaxHealth(5);
        this->SetCurrentHealth(this->GetMaxHealth());
        this->SetWeaponSocketOffset({0.0F, 0.0F});
        this->SetWeapon(std::make_shared<Bow>());
    }

    glm::vec2 GetMoveIntent() const override {
        if (this->m_AI != nullptr && this->m_AI->GetAttackTrigger()) {
            return {0.0F, 0.0F};
        }

        return Mob::GetMoveIntent();
    }
};

#endif
