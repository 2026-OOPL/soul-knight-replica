#ifndef FIGHT_ROOM_HPP
#define FIGHT_ROOM_HPP

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Generator/GenFightChamber.hpp"
#include "Generator/MapGenerator.hpp"
#include "Generator/RoomInfo.hpp"
#include <cstddef>
#include <memory>
#include <vector>

struct MonsterWave;

enum class WaveStatus {
    IDLE,
    FIGHTING,
    WAVE_CLEAR,
    FULL_CLEAR
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
    void Initialize(MapSystem* system) override;

    // Determine whether all wave in room were cleared
    bool IsRoomCleared(); 
    bool IsWaveCleared();
    void DebugClearRoom();
    
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

    std::vector<std::vector<SpawnInfo<MobType>>> m_MonsterWaves;
    std::vector<SpawnInfo<ObstacleType>> m_ObstacleSpawns;
};

#endif
