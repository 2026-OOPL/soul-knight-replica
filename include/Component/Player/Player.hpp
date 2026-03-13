#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include "Util/GameObject.hpp"
#include "Component/ICollidable.hpp"

class Player : Util::GameObject, ICollidable {
public:
    Player() : GameObject() {};

    bool isCollideWith(std::shared_ptr<ICollidable> other);
};

#endif