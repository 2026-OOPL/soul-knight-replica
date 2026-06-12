#include <array>
#include <algorithm>
#include <vector>
#include <memory>
#include <stdexcept>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Common/Random.hpp"
#include "Generator/MapGenerator.hpp"
#include "Generator/RoomInfo.hpp"
#include "Generator/GenFightChamber.hpp"

namespace {

void AddObstacle(
    const std::shared_ptr<RoomInfo> &info,
    ObstacleType type,
    const glm::vec2 &position
) {
    SpawnInfo<ObstacleType> spawn(type, position);
    info->AddSpawnObject(spawn);
}

void AddSymmetricObstaclePair(
    const std::shared_ptr<RoomInfo> &info,
    ObstacleType type,
    const glm::vec2 &position
) {
    AddObstacle(info, type, position);
    AddObstacle(info, type, {-position.x, -position.y});
}

void AddMirroredObstacleRow(
    const std::shared_ptr<RoomInfo> &info,
    ObstacleType type,
    float x,
    float y
) {
    AddObstacle(info, type, {-x, y});
    AddObstacle(info, type, {x, y});
    AddObstacle(info, type, {-x, -y});
    AddObstacle(info, type, {x, -y});
}

void AddRoomObstacleLayout(const std::shared_ptr<RoomInfo> &info) {
    switch (info->GetRoomType()) {
        case RoomType::ROOM_15_15:
            AddMirroredObstacleRow(info, ObstacleType::MECHANICAL_RUINS_PILLAR, 48.0F, 48.0F);
            AddSymmetricObstaclePair(info, ObstacleType::MECHANICAL_RUINS_FENCE, {0.0F, 72.0F});
            break;

        case RoomType::ROOM_17_17:
            AddSymmetricObstaclePair(info, ObstacleType::MECHANICAL_RUINS_BLOCK, {-64.0F, 0.0F});
            AddSymmetricObstaclePair(info, ObstacleType::MECHANICAL_RUINS_BLOCK, {0.0F, 64.0F});
            AddMirroredObstacleRow(info, ObstacleType::MECHANICAL_RUINS_PILLAR, 48.0F, 48.0F);
            break;

        case RoomType::ROOM_17_23:
            AddMirroredObstacleRow(info, ObstacleType::MECHANICAL_RUINS_FENCE, 48.0F, 96.0F);
            AddMirroredObstacleRow(info, ObstacleType::MECHANICAL_RUINS_PILLAR, 48.0F, 32.0F);
            AddSymmetricObstaclePair(info, ObstacleType::MECHANICAL_RUINS_BLOCK, {0.0F, 128.0F});
            break;

        case RoomType::ROOM_23_17:
            AddMirroredObstacleRow(info, ObstacleType::MECHANICAL_RUINS_FENCE, 96.0F, 48.0F);
            AddMirroredObstacleRow(info, ObstacleType::MECHANICAL_RUINS_PILLAR, 32.0F, 48.0F);
            AddSymmetricObstaclePair(info, ObstacleType::MECHANICAL_RUINS_BLOCK, {128.0F, 0.0F});
            break;

        case RoomType::ROOM_13_13:
            break;
    }
}

} // namespace

GenFightChamber::GenFightChamber(
    glm::ivec2 start,

    // The area limiter which tells generator whether this cooridinate could be use
    std::function<bool(glm::ivec2)> limiter,

    // The chamber count limiter
    int maxChamberCount,
    int minChamberCount,

    std::shared_ptr<MapBlueprint> blueprint,
    std::shared_ptr<RandomChoose> random
) : GenChamber(
    limiter,
    blueprint,
    random
) {
    this->m_Start = start;

    this->m_MinChamberCount = minChamberCount;
    this->m_MaxChamberCount = maxChamberCount;
    this->m_ObstacleRoomIndex = this->m_RandomChoose->GetInteger(
        std::max(1, this->m_MinChamberCount)
    );
}

void GenFightChamber::Generate() {
    glm::ivec2 currentPosition = m_Start;

    std::vector<glm::ivec2> candidateCooridinate;

    int i=0;

    do {
        candidateCooridinate = this->GetAvailableCooridinate();

        if (candidateCooridinate.size() == 0) {

            if (i+1 >= m_MinChamberCount) {
                // Exit the function if min count reached
                break;
            }

            throw std::runtime_error("There are no space for more room");
        }

        currentPosition = m_RandomChoose->ChooseFromVector(candidateCooridinate);
        
        this->CreateRoom(currentPosition);
        this->PopulateRoomContents(
            currentPosition,
            i == this->m_ObstacleRoomIndex
        );

        i++;
    } while (i < m_MaxChamberCount);
}

void GenFightChamber::CreateRoom(glm::ivec2 position) {
    RoomType type = this->m_RandomChoose->GetEnum<RoomType>();

    // Ensure the chamber size is not 13x13
    while (type == RoomType::ROOM_13_13) {
        type = this->m_RandomChoose->GetEnum<RoomType>();
    }

    std::shared_ptr<RoomInfo> info = std::make_shared<RoomInfo>(
        type,
        RoomPurpose::FIGHTING,
        m_RandomChoose
    );

    m_Blueprint->SetElementByCooridinate(position, info);
}

std::vector<glm::ivec2> GenFightChamber::GetAvailableCooridinate() {
    std::vector<glm::ivec2> chambers = this->m_Blueprint->GetChamberCooirdinateByPurpose(RoomPurpose::FIGHTING);

    std::vector<glm::ivec2> results;

    // Make start position the first priority to be choose
    if (this->m_Blueprint->GetElementByCooridinate(m_Start) == nullptr) {
        return {m_Start};
    }

    const glm::ivec2 directions[] = {
        glm::ivec2(0, 1),
        glm::ivec2(0, -1),
        glm::ivec2(1, 0),
        glm::ivec2(-1, 0),
    };

    for (auto const& i : chambers) {
        for (int j=0; j<4; j++) {
            glm::ivec2 newCooridinate = i + directions[j];

            if (!this->isCooridinateInBound(newCooridinate)) {
                continue;
            }

            if (this->m_Blueprint->GetElementByCooridinate(newCooridinate) != nullptr) {
                continue;
            }
            
            results.push_back(newCooridinate);
        }
    }

    return results;
}

void GenFightChamber::PopulateRoomContents(
    glm::ivec2 position,
    bool shouldAddObstacles
) {
    std::shared_ptr<RoomInfo> info = m_Blueprint->GetElementByCooridinate(position);

    if (shouldAddObstacles) {
        AddRoomObstacleLayout(info);
    }

    int waveCount = this->m_RandomChoose->GetInteger(1, 3); // 隨機 1 到 3 波
    const float safeDistance = 60.0F; // 怪物與箱子之間的最短安全距離 (可依照你的素材大小調整)
    
    for (int i = 0; i < waveCount; ++i) {
        std::vector<SpawnInfo<MobType>> wave;
        int monsterCount = this->m_RandomChoose->GetInteger(2, 5); // 每波 2 到 5 隻

        for (int j = 0; j < monsterCount; ++j) {
            glm::vec2 spawnPos;
            bool isValid = false;

            // 嘗試產生相對於房間中心的隨機位置，並檢查是否與箱子重疊 (最多嘗試 10 次避免無窮迴圈)
            for (int attempt = 0; attempt < 10; ++attempt) {
                spawnPos = info->GetRandomPositionInChamber();
                
                isValid = true;

                for (const auto& obs : info->GetObstacle()) {
                    if (glm::distance(spawnPos, obs.localPosition) < safeDistance) {
                        isValid = false;
                        break;
                    }
                }
                
                if (isValid) {
                    break; // 找到沒和箱子重疊的位置
                }
            }

            constexpr std::array<MobType, 7> kFightRoomMobTypes = {
                MobType::RUINS_GUARD,
                MobType::SHEAR_RUINS_GUARD,
                MobType::BOW_RUINS_GUARD,
                MobType::RUINS_SEARCHER,
                MobType::ANCIENT_GEAR_SET,
                MobType::RUINS_TURRET,
                MobType::PORTAL_MOB
            };
            const MobType mobType =
                kFightRoomMobTypes[this->m_RandomChoose->GetInteger(
                    static_cast<int>(kFightRoomMobTypes.size())
                )];

            SpawnInfo<MobType> spawn (
                mobType, spawnPos
            );
            
            wave.push_back(spawn);
        }
        
        info->AddMonsterWave(wave);
    }
}
