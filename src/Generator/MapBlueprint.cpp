#include <glm/fwd.hpp>
#include <memory>

#include <glm/vec2.hpp>
#include <stdexcept>

#include "Generator/MapBlueprint.hpp"
#include "Common/Enums.hpp"
#include "Generator/MapGenerator.hpp"

MapBlueprint::MapBlueprint(glm::vec2 size) {
    m_MapGridSize = size;

    m_MapGrid = std::vector<std::shared_ptr<RoomInfo>>(
        size.x * size.y, nullptr
    );
}

std::shared_ptr<RoomInfo> MapBlueprint::GetElementByCooridinate(glm::vec2 pos) {
    if (!this->isCooridinateInBound(pos)) {
        throw std::runtime_error("Position is out of range");
    }

    int index = m_MapGridSize.x * pos.x + pos.y;

    return this->m_MapGrid[index];
}

bool MapBlueprint::isCooridinateInBound(glm::vec2 pos) {
    if (pos.x >= this->m_MapGridSize.x) {
        return false;
    }

    if (pos.y >= this->m_MapGridSize.y) {
        return false;
    }

    if (pos.x < 0) {
        return false;
    }

    if (pos.y < 0) {
        return false;
    }

    return true;
}

void MapBlueprint::SetElementByCooridinate(glm::vec2 pos, std::shared_ptr<RoomInfo> info) {
    
    if (!this->isCooridinateInBound(pos)) {
        throw std::runtime_error("Position is out of range");
    }

    int index = m_MapGridSize.x * pos.x + pos.y;

    this->m_MapGrid[index] = info;
}

glm::vec2 MapBlueprint::GetSize() {
    return m_MapGridSize;
}

glm::vec2 MapBlueprint::GetCooridinateByElement(std::shared_ptr<RoomInfo> element) {
    for (int i=0; i<m_MapGridSize.y; i++) {
        for (int j=0; i<m_MapGridSize.x; j++) {
            int index = m_MapGridSize.x * i + j;
            if (m_MapGrid[index] == element) {
                return glm::vec2(i, j);
            }
        }
    }

    throw std::runtime_error("Element cannot be found");
}

std::vector<glm::vec2> MapBlueprint::GetAllFightChamberCooirdinate() {
    std::vector<glm::vec2> result;
    
    for (auto const& i : m_MapGrid) {
        if (i == nullptr) {
            continue;
        }

        if (i->roomPurpose != RoomPurpose::FIGHTING) {
            continue;
        }

        glm::vec2 cooridinate = this->GetCooridinateByElement(i);
        result.push_back(cooridinate);
    }
}