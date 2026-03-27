#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>
#include <vector>

#include "Component/Map/RoomAssembly.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    ~MapTest() override;
    void Update() override;

private:
    std::vector<std::shared_ptr<RoomAssembly>> m_RoomAssemblies;
    std::shared_ptr<RoomAssembly> m_MainRoomAssembly;
    std::shared_ptr<Player> m_MainPlayer;
    bool m_HasPlayerEnteredMainRoom = false;
    float m_DoorCloseDelayRemainingMs = -1.0F;
};

#endif
