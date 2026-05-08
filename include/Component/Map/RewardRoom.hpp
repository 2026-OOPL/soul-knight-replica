#ifndef REWARD_ROOM_HPP
#define REWARD_ROOM_HPP

#include <memory>

#include "Component/Map/BaseRoom.hpp"
#include "Generator/RoomInfo.hpp"

class RewardRoom : public BaseRoom {
public:
    RewardRoom(
        const glm::vec2 &absolutePosition,
        const DoorConfig &doorConfig,
        const std::shared_ptr<RoomInfo> &info,
        float wallThickness
    );

    ~RewardRoom() override = default;

    void Initialize(MapSystem *mapSystem) override;
};

#endif
