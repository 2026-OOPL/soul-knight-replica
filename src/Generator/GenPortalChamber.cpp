#include <algorithm>
#include <glm/vec2.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Generator/MapGenerator.hpp"
#include "Generator/GenPortalChamber.hpp"
#include "Generator/MapBlueprint.hpp"
#include "Common/Enums.hpp" 

GenPortalChamber::GenPortalChamber(
    GeneratePolicy generatePolicy,

    // The area limiter which tells generator whether this cooridinate could be use
    std::function<bool(glm::ivec2)> limiter,

    std::shared_ptr<MapBlueprint> blueprint,
    std::shared_ptr<RandomChoose> random    
) : GenChamber(
    limiter,
    blueprint,
    random    
) {
    this->m_GeneratePolicy = generatePolicy;
}

void GenPortalChamber::Generate() {
    std::vector<glm::ivec2> cooridinates = this->GetAvailableCooridinate();
 
    if (cooridinates.empty()) {
        throw std::runtime_error("Portal room cannot be generated");
    }
    
    std::shared_ptr<RoomInfo> info = std::make_shared<RoomInfo>(
        RoomType::ROOM_13_13, RoomPurpose::PORTAL
    );

    m_Blueprint->SetElementByCooridinate(cooridinates[0], info);
}

std::vector<glm::ivec2> GenPortalChamber::GetAvailableCooridinate() {
    std::vector<glm::ivec2> candidate;

    std::vector<glm::ivec2> chambers = this->m_Blueprint->GetAllChamberCooirdinate();

    const glm::ivec2 directions[] = {
        glm::ivec2(0, 1),
        glm::vec2(0, -1),
        glm::ivec2(1, 0),
        glm::ivec2(-1, 0),
    };

    for (auto const& i : chambers) {
        for (int j=0; j<4; j++) {
            glm::ivec2 newCooridinate = i + directions[j];

            if (!this->isCooridinateInBound(newCooridinate)) {
                continue;
            }

            if (this->m_Blueprint->GetElementByCooridinate(newCooridinate) != nullptr ) {
                continue;
            }
            
            candidate.push_back(newCooridinate);
        }
    }

    std::sort(
        candidate.begin(),
        candidate.end(),
        [this](glm::vec2 a, glm::vec2 b) {
            return this->SortFunction(a, b); // 如果 a > b，則 a 應該排在前面
        }
    );

    return candidate;
}

bool GenPortalChamber::SortFunction(glm::ivec2 a, glm::ivec2 b) {
    switch (m_GeneratePolicy) {
        case GeneratePolicy::TOP_LEFT:
            // 找最小 Y (Top)，若 Y 相同則找最小 X (Left)
            if (a.y != b.y) return a.y < b.y;
            return a.x < b.x;

        case GeneratePolicy::TOP_RIGHT:
            // 找最小 Y (Top)，若 Y 相同則找最大 X (Right)
            if (a.y != b.y) return a.y < b.y;
            return a.x > b.x;

        case GeneratePolicy::BOTTOM_LEFT:
            // 找最大 Y (Bottom)，若 Y 相同則找最小 X (Left)
            if (a.y != b.y) return a.y > b.y;
            return a.x < b.x;

        case GeneratePolicy::BOTTOM_RIGHT:
            // 找最大 Y (Bottom)，若 Y 相同則找最大 X (Right)
            if (a.y != b.y) return a.y > b.y;
            return a.x > b.x;
            
        default:
            return false;
    }
}