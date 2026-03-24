#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    ~MapTest() override;
    void Update() override;

private:
    Collision::CollisionSystem m_CollisionSystem;
    std::vector<std::shared_ptr<Door>> m_TestDoors;
    std::shared_ptr<Player> m_MainPlayer;
    std::shared_ptr<BaseRoom> m_MainRoom;
    bool m_HasPlayerEnteredMainRoom = false;
    float m_DoorCloseDelayRemainingMs = -1.0F;
};

#endif
