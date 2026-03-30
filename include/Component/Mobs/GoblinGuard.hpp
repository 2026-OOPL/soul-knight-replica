#ifndef MOB_HPP
#define MOB_HPP

#include "Common/MapObject.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Player/Player.hpp"
#include <vector>

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

class GoblinGuard : public Player {
public:
    GoblinGuard() : Player(
        STAND_SPRITE,
        WALK_SPRITE,
        DIE_SPRITE
    ) {};

    glm::vec2 GetMoveIntent() const override;
};

#endif