#ifndef GOBLIN_GUARD_HPP
#define GOBLIN_GUARD_HPP

#include <memory>
#include <vector>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"

namespace {
    const std::vector<std::string> WALK_SPRITE = {
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_0.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_1.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_2.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_3.png"
    };

    const std::vector<std::string> STAND_SPRITE = {
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_0.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_1.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_2.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_3.png"
    };

    const std::vector<std::string> DIE_SPRITE = {
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_die.png"
    };
}

class GoblinGuard : public Mob {
public:
    GoblinGuard(
        std::shared_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Mob(
        STAND_SPRITE,
        WALK_SPRITE,
        DIE_SPRITE,
        tracePlayer,
        collisionSystem
    ) {
        this->SetAttackAnimation(std::make_shared<Util::Animation>(
            STAND_SPRITE,
            false,
            30,
            false,
            0,
            false
        ));
    };

};

#endif
