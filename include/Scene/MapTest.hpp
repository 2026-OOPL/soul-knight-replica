#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>
#include <vector>

#include "Component/Map/MapBase.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Component/Player/Player.hpp"

class MapTest : public MapBase {
public:
    MapTest(Util::Renderer *root, glm::vec2 cooridinate,
            std::vector<std::shared_ptr<MapPiece>> pieces)
        : MapBase(root, cooridinate, pieces) {
        m_Pieces = pieces;
        m_Cooridinate = cooridinate;
        Initialize();
    }

    ~MapTest() override { Dispose(); }

    void Initialize() override;
    void Dispose() override;
    void Update() override;

private:
    bool WillPlayerCollide(const glm::vec2 &nextCoordinate) const;

private:
    glm::vec2 m_Cooridinate;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;
    std::shared_ptr<Player> m_Player;
    std::shared_ptr<MapPiece> m_TestBlock;
    float m_PlayerSpeed = 0.35F;
};

#endif
