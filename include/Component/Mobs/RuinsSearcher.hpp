#ifndef RUINS_SEARCHER_HPP
#define RUINS_SEARCHER_HPP

#include <memory>
#include <vector>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"

namespace {
    const std::vector<std::string> RUINS_SEARCHER_WALK_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_walk_0.png",
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_walk_1.png",
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_walk_2.png",
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_walk_3.png"
    };

    const std::vector<std::string> RUINS_SEARCHER_STAND_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_stand_0.png",
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_stand_1.png"
    };

    const std::vector<std::string> RUINS_SEARCHER_DIE_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsSearcher/RuinsSearcher_die.png"
    };
}

class RuinsSearcher : public Mob {
public:
    RuinsSearcher(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Mob(
            RUINS_SEARCHER_STAND_SPRITE,
            RUINS_SEARCHER_WALK_SPRITE,
            RUINS_SEARCHER_DIE_SPRITE,
            tracePlayer,
            collisionSystem
        ) {
        this->SetMaxHealth(12);
        this->SetCurrentHealth(this->GetMaxHealth());
        this->SetWeapon(nullptr);
        this->SetMeleeAttack(4, 8.0F, 700.0F, 0.12F);
    }
};

#endif
