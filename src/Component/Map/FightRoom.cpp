#include <memory>

#include "Util/Logger.hpp"

#include "Component/Map/FightRoom.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Generator/MapGenerator.hpp"
#include "Component/Mobs/GoblinGuard.hpp"

FightRoom::FightRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness
)
: BaseRoom(
    absolutePosition,
    info->roomType,
    RoomPurpose::FIGHTING,
    doorConfig,
    BaseRoom::BuildWallConfigFromDoorConfig(
        doorConfig,
        wallThickness,
        info->roomType,
        RoomPurpose::FIGHTING
    )
) {
    this->OpenAllDoors();

    for (auto const& i : info->obstacles) {
        // TODO: Implement boxes creation
    }

    this->m_MonsterWaves = info->monsterWaves;

    this->m_CompletedWave = 0;
    this->m_MaxMobWave = info->monsterWaves.size();

}

void FightRoom::OnPlayerEnter() {
    m_PlayerInside = true;
}

void FightRoom::OnPlayerLeave() {
    m_PlayerInside = false;
}

void FightRoom::SetMapSystem(MapSystem* system) {
    this->m_MapSystem = system;
}

void FightRoom::Update() {
    if (! m_PlayerInside) {
        return;
    }

    switch (m_WaveStatus) {
        case WaveStatus::IDLE:
            this->CloseAllDoors();
            this->StartNextMonsterWave();
            LOG_INFO("Player entered a room, start spawning mobs.");
            m_WaveStatus = WaveStatus::FIGHTING;
            break;

        case WaveStatus::FIGHTING:
            if (this->IsWaveCleared()) {
                m_WaveStatus = WaveStatus::WAVE_CLEAR;
            }
            break;
        
        case WaveStatus::WAVE_CLEAR:
            m_CompletedWave++;
            LOG_INFO("Wave " +  std::to_string(m_CompletedWave) +" cleared.");
            if (this->m_CompletedWave < m_MaxMobWave) {    
                this->StartNextMonsterWave();
                m_WaveStatus = WaveStatus::FIGHTING;
            } else {
                m_WaveStatus = WaveStatus::FULL_CLEAR;
            }
            break;

        case WaveStatus::FULL_CLEAR:
            LOG_INFO("Room is fully cleared.");
            this->OpenAllDoors();
            break;
    }
    
    BaseRoom::Update();
}

bool FightRoom::IsRoomCleared() {
    return m_CompletedWave >= m_MaxMobWave;
}

bool FightRoom::IsWaveCleared() {
    return m_MapSystem->GetMob().empty();
}

void FightRoom::StartNextMonsterWave() {
    if (m_MapSystem == NULL) {
        LOG_WARN("Failed to start next wave, no map system bound.");
        return;
    }

    if (m_CompletedWave >= m_MaxMobWave) {
        LOG_WARN("Failed to start next wave, max waves exceeded.");
        return;
    }

    std::weak_ptr<Character> target;

    if (m_MapSystem->GetPlayers().empty()) {
        LOG_WARN("Failed to start next wave, target player not found.");
        return;
    }
    
    target = m_MapSystem->GetPlayers().front();
    
    MonsterWave wave = m_MonsterWaves[m_CompletedWave];

    for (auto const &i : wave.monsters) {
        std::shared_ptr<Character> mob;

        switch (static_cast<MonsterType>(i.type)) {
            case MonsterType::GOBLIN_GUARD:
                mob = std::make_shared<GoblinGuard>(target, m_MapSystem->GetCollisionSystem());
                break;
                
            default:
                LOG_WARN("Failed to add mob, unknown monster type.");
                break;
        }

        if (mob != nullptr) {
            mob->SetAbsoluteTranslation(this->GetAbsoluteTranslation() + i.localPosition); // 加上房間絕對座標
            this->AddMob(mob);              // 讓 BaseRoom 記錄這隻怪，用來判斷房間是否清空
            this->m_MapSystem->AddMob(mob); // 將怪物註冊到 MapSystem，確保碰撞和更新正常運作
        }
    }

    m_WaveStatus = WaveStatus::FIGHTING;
}