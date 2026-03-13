#ifndef MAP_HPP
#define MAP_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/fwd.hpp>
#include <vector>

class MapPiece : Util::GameObject {
    glm::vec2 offset;

private:
    Util::Image background;
};

class Map : Util::GameObject{
    Map() : Util::GameObject() {
    }
};

#endif