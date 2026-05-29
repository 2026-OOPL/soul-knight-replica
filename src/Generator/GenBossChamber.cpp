#include "Generator/GenBossChamber.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Common/Random.hpp"
#include "Generator/MapBlueprint.hpp"
#include "Generator/RoomInfo.hpp"

namespace {

bool ContainsCoordinate(
    const std::vector<glm::ivec2> &coordinates,
    const glm::ivec2 &candidate
) {
    return std::find(coordinates.begin(), coordinates.end(), candidate) !=
           coordinates.end();
}

} // namespace

GenBossChamber::GenBossChamber(
    std::function<bool(glm::ivec2)> limiter,
    std::shared_ptr<MapBlueprint> blueprint,
    std::shared_ptr<RandomChoose> random
) : GenChamber(limiter, blueprint, random) {
}

void GenBossChamber::Generate() {
    std::vector<glm::ivec2> candidates = this->GetAvailableCooridinate();
    if (candidates.empty()) {
        throw std::runtime_error("Boss room cannot be generated");
    }

    const glm::ivec2 coordinate = this->m_RandomChoose->ChooseFromVector(candidates);
    std::shared_ptr<RoomInfo> info = std::make_shared<RoomInfo>(
        RoomType::ROOM_23_17,
        RoomPurpose::BOSS,
        this->m_RandomChoose
    );

    std::vector<SpawnInfo<MobType>> bossWave;
    bossWave.emplace_back(MobType::ZULAN_IN_RUINS, glm::vec2(0.0F, 0.0F));
    info->AddMonsterWave(bossWave);

    this->m_Blueprint->SetElementByCooridinate(coordinate, info);
}

std::vector<glm::ivec2> GenBossChamber::GetAvailableCooridinate() {
    std::vector<glm::ivec2> result;
    const std::vector<glm::ivec2> fightRooms =
        this->m_Blueprint->GetAllFightChamberCooirdinate();

    const glm::ivec2 directions[] = {
        glm::ivec2(0, 1),
        glm::ivec2(0, -1),
        glm::ivec2(1, 0),
        glm::ivec2(-1, 0)
    };

    for (const glm::ivec2 &fightRoom : fightRooms) {
        for (const glm::ivec2 &direction : directions) {
            const glm::ivec2 candidate = fightRoom + direction;

            if (!this->isCooridinateInBound(candidate)) {
                continue;
            }

            if (this->m_Blueprint->GetElementByCooridinate(candidate) != nullptr) {
                continue;
            }

            if (ContainsCoordinate(result, candidate)) {
                continue;
            }

            result.push_back(candidate);
        }
    }

    return result;
}
