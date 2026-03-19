#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>

#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();

    ~MapTest() override;

    void Initialize() override;
    void Dispose() override;
    void Update() override;

private:
    std::shared_ptr<Collision::CollisionSystem> m_CollisionSystem;
    std::shared_ptr<Player> m_MainPlayer;
    // std::shared_ptr<MapPiece> m_TestBlock;
};

#endif
