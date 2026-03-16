#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Component/Map/MapBase.hpp"
#include "Component/Map/MapPiece.hpp"

class MapTest : public MapBase {
public:
    MapTest(glm::vec2 cooridinate) : MapBase(cooridinate, {}) {
        m_Transform.scale = {1.5, 1.5};
        this->m_Pieces = pieces;
        this->m_Cooridinate = cooridinate;
    }

    ~MapTest() override {
        Dispose();
    };

    void Initialize() override;
    void Dispose() override;

    void Update() override;

    glm::vec2 m_Cooridinate;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;

protected:
    // std::unique_ptr<Util::GameObject> m_Character = 
};

#endif