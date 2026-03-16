#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Component/Map/MapBase.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Component/Player/Player.hpp"


class MapTest : public MapBase {
public:
    MapTest(glm::vec2 cooridinate) : MapBase(cooridinate, {}) {
        m_Transform.scale = {1.5, 1.5};
        this->m_Pieces = pieces;
        this->m_Cooridinate = cooridinate;
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
