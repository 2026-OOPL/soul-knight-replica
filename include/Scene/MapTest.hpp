#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <glm/vec2.hpp>
#include <memory>

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    ~MapTest() override;
    void Update() override;

private:
    std::shared_ptr<Player> m_MainPlayer;
    std::shared_ptr<BaseRoom> m_MainRoom;
};

#endif
