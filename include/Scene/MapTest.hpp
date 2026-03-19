#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>

#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();

    ~MapTest() override;

    void Update() override;

private:
    std::shared_ptr<Player> m_MainPlayer;
    // std::shared_ptr<MapPiece> m_TestBlock;
};

#endif
