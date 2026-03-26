#include <functional>
#include <glm/fwd.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Common/Random.hpp"
#include "Generator/MapBlueprint.hpp"
#include "Generator/GenRewardChamber.hpp"

GenRewardChamber::GenRewardChamber(

    // The area limiter which tells generator whether this cooridinate could be use
    std::function<bool(glm::vec2)> limiter,

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
    this->m_MaxChamberCount = maxChamberCount;
    this->m_MinChamberCount = minChamberCount;
}

void GenRewardChamber::Generate() {
    std::vector<glm::vec2> candidateCooridinate = this->GetAvailableCooridinate();
    glm::vec2 currentPosition = m_RandomChoose->ChooseFromVector(candidateCooridinate);

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

std::vector<glm::vec2> GenRewardChamber::GetAvailableCooridinate() {
    std::vector<glm::vec2> result;
    std::vector<glm::vec2> fightChamber = m_Blueprint->GetAllFightChamberCooirdinate();   
    
    const glm::vec2 direction[] = {
        glm::vec2(0, 1),
        glm::vec2(0, -1),
        glm::vec2(1, 0),
        glm::vec2(-1, 0)
    };

    for (auto const& coor : fightChamber) {
        for (int i=0; i<4; i++) {
            glm::vec2 newCooridinate = coor + direction[i];

            if (! this->isCooridinateInBound(newCooridinate)) {
                continue;
            }

            if (m_Blueprint->GetElementByCooridinate(newCooridinate) != nullptr) {
                continue;
            }

            result.push_back(newCooridinate);
        }
    }

    return result;
}