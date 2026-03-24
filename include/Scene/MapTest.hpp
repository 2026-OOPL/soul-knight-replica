#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>

#include "Component/Map/RoomAssembly.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    ~MapTest() override;
    void Update() override;

private:
    Collision::CollisionSystem m_CollisionSystem;
    std::unique_ptr<RoomAssembly> m_MainRoomAssembly;
    std::shared_ptr<Player> m_MainPlayer;
    bool m_HasPlayerEnteredMainRoom = false;
    float m_DoorCloseDelayRemainingMs = -1.0F;
};

#endif
