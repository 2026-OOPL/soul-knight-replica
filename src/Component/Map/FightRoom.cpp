#include <memory>

#include "Generator/RoomInfo.hpp"
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
    info->GetRoomType(),
    RoomPurpose::FIGHTING,
    doorConfig,
    BaseRoom::BuildWallConfigFromDoorConfig(
        doorConfig,
        wallThickness,
        info->GetRoomType(),
        RoomPurpose::FIGHTING
    )
) {
    this->OpenAllDoors();

    for (auto const& i : info->GetObstacle()) {
        // TODO: Implement boxes creation
    }

    this->m_MonsterWaves = info->GetMonsterWaves();

    this->m_CompletedWave = 0;
    this->m_MaxMobWave = this->m_MonsterWaves.size();

}

void FightRoom::OnPlayerEnter() {
    m_PlayerInside = true;

    if (m_WaveStatus == WaveStatus::IDLE) {
        this->CloseAllDoors();
        m_WaveStatus = WaveStatus::FIGHTING;

        for (auto const& i : this->GetMobs()) {
            i->m_AI->UnFreeze();
        }

        LOG_INFO("Player entered a room, start spawning mobs.");
    }

}

void FightRoom::OnPlayerLeave() {
    m_PlayerInside = false;
}

void FightRoom::Update() {
    if (! m_PlayerInside) {
        return;
    }

    switch (m_WaveStatus) {
        case WaveStatus::IDLE:
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
                LOG_INFO("Room is fully cleared.");
                m_WaveStatus = WaveStatus::FULL_CLEAR;
            }
            break;

        case WaveStatus::FULL_CLEAR:
            this->OpenAllDoors();
            break;
    }
    
    BaseRoom::Update();
}

bool FightRoom::IsRoomCleared() {
    return m_CompletedWave >= m_MaxMobWave;
}

bool FightRoom::IsWaveCleared() {
    for (auto const& i : this->GetMobs()) {
        if (i->IsDead()) continue;
        return false;
    }

    return true;
}

void FightRoom::StartNextMonsterWave() {
    if (m_MapSystem == NULL) {
        LOG_ERROR("Failed to start next wave, no map system bound.");
        return;
    }

    if (m_CompletedWave >= m_MaxMobWave) {
        LOG_ERROR("Failed to start next wave, max waves exceeded.");
        return;
    }

    std::weak_ptr<Character> target;

    if (m_MapSystem->GetPlayers().empty()) {
        LOG_ERROR("Failed to start next wave, target player not found.");
        return;
    }
    
    target = m_MapSystem->GetPlayers().front();
    
    std::vector<SpawnInfo<MobType>>& wave = m_MonsterWaves[m_CompletedWave];

    for (auto const &i : wave) {
        std::shared_ptr<Mob> mob;

        switch (i.type) {
            case MobType::GOBLIN_GUARD:
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
}

void FightRoom::Initialize(MapSystem* system) {
    this->m_MapSystem = system;

    // Spawn the mobs in the first wave 
    this->StartNextMonsterWave();

    // Freeze the mob in the first place 
    for (auto const& i : this->GetMobs()) {
        i->m_AI->Freeze();
    }
}