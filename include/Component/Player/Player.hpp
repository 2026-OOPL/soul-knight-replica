#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include <nlohmann/detail/iterators/primitive_iterator.hpp>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"

class Player : public Character {
public:
    Player(
        const std::vector<std::string>& StandSprite,
        const std::vector<std::string>& WalkSprite,
        const std::vector<std::string>& DieSprite
    );

    virtual glm::vec2 GetMoveIntent() const override;
};

#endif
