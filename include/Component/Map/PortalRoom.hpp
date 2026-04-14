#ifndef PORTAL_ROOM_HPP
#define PORTAL_ROOM_HPP

#include <memory>

#include "Common/Enums.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Generator/RoomInfo.hpp"

class PortalRoom : public BaseRoom {
public:
    PortalRoom(
        const glm::vec2 &absolutePosition,
        const DoorConfig &doorConfig,
        const std::shared_ptr<RoomInfo> &info,
        float wallThickness
    );

    virtual ~PortalRoom() override = default;

    // Record current state of whether player is in the room
    void OnPlayerEnter() override;
    void OnPlayerLeave() override;

    // IStateful override
    void Update() override;

    // Do portal generation on initialize
    void Initialize(MapSystem *mapSystem) override;
    
protected:
    void StartNextMonsterWave();


private:

};

#endif