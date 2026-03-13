#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include "Component/Map/MapBase.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Util/GameObject.hpp"

class MapTest : public MapBase {
public:
    MapTest(Util::Renderer* m_Root, glm::vec2 cooridinate, std::vector<std::shared_ptr<MapPiece>> pieces) : MapBase(m_Root, cooridinate, pieces) {
        this->m_Pieces = pieces;
        this->m_Cooridinate = cooridinate;
        Initialize();
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