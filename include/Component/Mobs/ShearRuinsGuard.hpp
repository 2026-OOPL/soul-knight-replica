#ifndef SHEAR_RUINS_GUARD_HPP
#define SHEAR_RUINS_GUARD_HPP

#include <memory>
#include <vector>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Component/Weapons/Shear.hpp"

namespace {
    const std::vector<std::string> RUINS_GUARD_WALK_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_1.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_2.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_3.png"
    };

    const std::vector<std::string> RUINS_GUARD_STAND_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_1.png"
    };

    const std::vector<std::string> RUINS_GUARD_DIE_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_die.png"
    };
}

class ShearRuinsGuard : public Mob {
public:
    ShearRuinsGuard(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Mob(
            RUINS_GUARD_STAND_SPRITE,
            RUINS_GUARD_WALK_SPRITE,
            RUINS_GUARD_DIE_SPRITE,
            tracePlayer,
            collisionSystem
        ) {
        this->SetAttackAnimation(std::make_shared<Util::Animation>(
            RUINS_GUARD_STAND_SPRITE,
            false,
            30,
            false,
            0,
            false
        ));
        this->SetMaxHealth(5);
        this->SetCurrentHealth(this->GetMaxHealth());
        this->SetWeapon(std::make_shared<Shear>());
    }
};

#endif
