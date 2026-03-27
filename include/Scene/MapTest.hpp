#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>
#include <vector>

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    ~MapTest() override;
    void Update() override;

private:
    std::vector<std::shared_ptr<BaseRoom>> m_RoomsInScene;
    std::shared_ptr<BaseRoom> m_MainRoom;
    std::shared_ptr<Player> m_MainPlayer;
};

#endif
