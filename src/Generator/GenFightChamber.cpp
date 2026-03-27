#include <vector>
#include <memory>
#include <stdexcept>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Common/Random.hpp"
#include "Generator/MapGenerator.hpp"
#include "Generator/GenFightChamber.hpp"

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
        type, RoomPurpose::FIGHTING
    );

    m_Blueprint->SetElementByCooridinate(position, info);
}

std::vector<glm::ivec2> GenFightChamber::GetAvailableCooridinate() {
    std::vector<glm::ivec2> chambers = this->m_Blueprint->GetAllFightChamberCooirdinate();

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