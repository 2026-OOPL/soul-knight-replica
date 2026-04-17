#ifndef PLAYER_KINGHT_HPP
#define PLAYER_KINGHT_HPP


#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <glm/vec2.hpp>
#include "Component/Player/Player.hpp"
#include "Util/Animation.hpp"
#include "Util/Transform.hpp"

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"

namespace KnightPlayer {

constexpr int MAX_HEALTH = 10;
constexpr int MAX_SHIELD = 5;
constexpr int MAX_AMMO = 100;

const std::vector<std::string> WALK_SPRITES = {
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_0.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_1.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_2.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_3.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_4.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_5.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_6.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_walk_7.png"
};

const std::vector<std::string> STAND_SPRITES = {
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_0.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_1.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_2.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_3.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_4.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_5.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_6.png",
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_stand_7.png"
};

const std::vector<std::string> DIE_SPRITES = {
    std::string(RESOURCE_DIR) + "/Character/Knight/knight_die.png",
};
}

class Knight : public Player {
public:
    Knight(std::function<std::shared_ptr<Character>()> GetNearestMob);

    glm::vec2 GetFaceDirection() const override;
    
private:
    
    std::function<std::shared_ptr<Character>()> m_GetNearestMob;
};

#endif
