#ifndef BOSS_ROOM_HPP
#define BOSS_ROOM_HPP

#include <memory>

#include "Component/Map/FightRoom.hpp"
#include "Generator/RoomInfo.hpp"

class BossRoom : public FightRoom {
public:
    BossRoom(
        const glm::vec2 &absolutePosition,
        const DoorConfig &doorConfig,
        const std::shared_ptr<RoomInfo> &info,
        float wallThickness
    );

    ~BossRoom() override = default;

    void Initialize(MapSystem *mapSystem) override;

private:
    void SpawnCornerObstacles(MapSystem *mapSystem);
};

#endif
