#include <algorithm>
#include <memory>

#include "Component/Mobs/GoblinGuard.hpp"
#include "Generator/RoomInfo.hpp"
#include "Util/Logger.hpp"

#include "Component/Map/FightRoom.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Mobs/AncientGearSet.hpp"
#include "Component/Mobs/BowRuinsGuard.hpp"
#include "Component/Mobs/Ghost.hpp"
#include "Component/Mobs/GhostKing.hpp"
#include "Component/Mobs/PortalMob.hpp"
#include "Component/Mobs/RuinsGuard.hpp"
#include "Component/Mobs/RuinsSearcher.hpp"
#include "Component/Mobs/RuinsTurret.hpp"
#include "Component/Mobs/ShearRuinsGuard.hpp"
#include "Component/Mobs/VitaminCMecha.hpp"
#include "Component/Mobs/ZulanInRuins.hpp"

namespace {

constexpr float kDoorPlayerPushPadding = 2.0F;

glm::vec2 GetDoorInwardNormal(DoorSide side) {
    switch (side) {
    case DoorSide::Top:
        return {0.0F, -1.0F};
    case DoorSide::Right:
        return {-1.0F, 0.0F};
    case DoorSide::Bottom:
        return {0.0F, 1.0F};
    case DoorSide::Left:
        return {1.0F, 0.0F};
    }

    return {0.0F, 0.0F};
}

float GetAxisExtent(const glm::vec2 &size, const glm::vec2 &axis) {
    return std::abs(axis.x) > 0.0F ? size.x * 0.5F : size.y * 0.5F;
}

} // namespace

FightRoom::FightRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness
)
: FightRoom(
    absolutePosition,
    doorConfig,
    info,
    wallThickness,
    RoomPurpose::FIGHTING
) {
}

FightRoom::FightRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness,
    RoomPurpose purpose
)
: BaseRoom(
    absolutePosition,
    info->GetRoomType(),
    purpose,
    doorConfig,
    BaseRoom::BuildWallConfigFromDoorConfig(
        doorConfig,
        wallThickness,
        info->GetRoomType(),
        purpose
    )
) {
    this->OpenAllDoors();

    this->m_MonsterWaves = info->GetMonsterWaves();

    this->m_CompletedWave = 0;
    this->m_MaxMobWave = this->m_MonsterWaves.size();

}

void FightRoom::OnPlayerEnter() {
    m_PlayerInside = true;

    if (m_WaveStatus == WaveStatus::IDLE) {
        this->CloseAllDoors();
        this->PushPlayersInsideClosedDoors();
        m_WaveStatus = WaveStatus::FIGHTING;

        for (auto const& i : this->GetMobs()) {
            i->SetDamageEnabled(true);
            i->m_AI->UnFreeze();
        }

        LOG_DEBUG("Player entered a room, start spawning mobs.");
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

void FightRoom::PushPlayersInsideClosedDoors() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    Collision::CollisionSystem collisionSystem;

    for (const auto &player : this->m_MapSystem->GetPlayers()) {
        if (player == nullptr || player->IsDead()) {
            continue;
        }

        glm::vec2 playerPosition = player->GetAbsoluteTranslation();
        Collision::AxisAlignedBox playerBox = player->GetCollisionBoxAt(playerPosition);

        for (const auto &door : this->GetDoors()) {
            if (door == nullptr || door->IsOpen()) {
                continue;
            }

            const std::vector<Collision::CollisionPrimitive> doorPrimitives =
                door->CollectBlockingPrimitives();
            for (const auto &primitive : doorPrimitives) {
                if (!collisionSystem.IsOverlapping(playerBox, primitive.box)) {
                    continue;
                }

                const glm::vec2 inwardNormal = GetDoorInwardNormal(door->GetSide());
                const float playerHalfExtent =
                    GetAxisExtent(playerBox.size, inwardNormal);
                const float doorHalfExtent =
                    GetAxisExtent(primitive.box.size, inwardNormal);
                const float requiredDepth =
                    playerHalfExtent + doorHalfExtent + kDoorPlayerPushPadding;
                const float currentDepth =
                    glm::dot(playerPosition - primitive.box.center, inwardNormal);
                const float pushDistance =
                    std::max(0.0F, requiredDepth - currentDepth);

                if (pushDistance <= 0.0F) {
                    continue;
                }

                playerPosition += inwardNormal * pushDistance;
                player->SetAbsoluteTranslation(playerPosition);
                playerBox = player->GetCollisionBoxAt(playerPosition);
            }
        }
    }
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

void FightRoom::DebugClearRoom() {
    for (const auto &mob : this->GetMobs()) {
        if (mob == nullptr || mob->IsDead()) {
            continue;
        }

        mob->SetDamageEnabled(true);
        mob->ApplyDamage(mob->GetCurrentHealth());
    }

    this->m_CompletedWave = this->m_MaxMobWave;
    this->m_WaveStatus = WaveStatus::FULL_CLEAR;
    this->OpenAllDoors();
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

    std::shared_ptr<Character> target;

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

            case MobType::RUINS_GUARD:
                mob = std::make_shared<RuinsGuard>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::SHEAR_RUINS_GUARD:
                mob = std::make_shared<ShearRuinsGuard>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::BOW_RUINS_GUARD:
                mob = std::make_shared<BowRuinsGuard>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::RUINS_SEARCHER:
                mob = std::make_shared<RuinsSearcher>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::ANCIENT_GEAR_SET:
                mob = std::make_shared<AncientGearSet>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::RUINS_TURRET:
                mob = std::make_shared<RuinsTurret>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::PORTAL_MOB:
                mob = std::make_shared<PortalMob>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::GHOST:
                mob = std::make_shared<Ghost>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::GHOST_KING:
                mob = std::make_shared<GhostKing>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::ZULAN_IN_RUINS:
                mob = std::make_shared<ZulanInRuins>(target, m_MapSystem->GetCollisionSystem());
                break;

            case MobType::VITAMIN_C_MECHA:
                mob = std::make_shared<VitaminCMecha>(target, m_MapSystem->GetCollisionSystem());
                break;
                
            default:
                LOG_WARN("Failed to add mob, unknown monster type.");
                break;
        }

        if (mob != nullptr) {
            mob->SetDamageEnabled(m_PlayerInside);
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
        i->SetDamageEnabled(false);
        i->m_AI->Freeze();
    }
}
