#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    ~MapTest() override;
    void Update() override;

private:
    void CloseAllDoors();

    std::shared_ptr<Collision::CollisionSystem> m_CollisionSystem;
    std::shared_ptr<Player> m_MainPlayer;
    std::shared_ptr<BaseRoom> m_MainRoom;
    std::vector<std::shared_ptr<Door>> m_Doors;
    glm::vec2 m_DoorOpeningSize = {0.0F, 0.0F};
    float m_DoorCloseDelayMs = 2000.0F;
    float m_DoorCloseElapsedMs = 0.0F;
    bool m_IsDoorCloseScheduled = false;
    bool m_HasClosedDoors = false;
};

#endif
