#ifndef FIGHT_ROOM_HPP
#define FIGHT_ROOM_HPP

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Generator/MapGenerator.hpp"
#include <cstddef>
#include <memory>
#include <vector>

enum class WaveStatus {
    IDLE,
    FIGHTING,
    CLEAR
};

class FightRoom : public BaseRoom {
public:
    FightRoom(
        const glm::vec2 &absolutePosition,
        const DoorConfig &doorConfig,
        const std::shared_ptr<RoomInfo> &info,
        float wallThickness
    );

    // Record current state of whether player is in the room
    void OnPlayerEnter() override;
    void OnPlayerLeave() override;

    // MapSystem pointer is needed to be bound after the initialization
    void SetMapSystem(MapSystem* system);

    // Determine whether all wave in room were cleared
    bool IsRoomCleared(); 
    bool IsWaveCleared();
    
    // IStateful override
    void Update() override;
    
protected:
    void StartNextMonsterWave();

    WaveStatus m_WaveStatus = WaveStatus::IDLE;

private:
    MapSystem* m_MapSystem = NULL;

    bool m_HasTriggeredLockdown = false;

    bool m_PlayerInside = false;

    int m_MaxMobWave = 0;
    int m_CompletedWave = 0;

    std::vector<MonsterWave> m_MonsterWaves;
};

#endif
