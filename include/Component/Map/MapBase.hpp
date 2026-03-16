#ifndef MAP_BASE_HPP
#define MAP_BASE_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Scene.hpp"
#include "Component/Map/MapPiece.hpp"

class MapBase : public Scene {
public:
    MapBase(glm::vec2 cooridinate, std::vector<std::shared_ptr<MapPiece>> pieces) : Scene() {
        this->pieces = pieces;
        this->m_Cooridinate = cooridinate;
    }

    virtual ~MapBase() = default;

    glm::vec2 m_Cooridinate;
    std::vector<std::shared_ptr<MapPiece>> pieces;

protected:
    // std::unique_ptr<Util::GameObject> m_Character = 
};

#endif