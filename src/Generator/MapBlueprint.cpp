#include <memory>

#include <glm/vec2.hpp>
#include <stdexcept>

#include "Generator/MapBlueprint.hpp"
#include "Generator/MapGenerator.hpp"

MapBlueprint::MapBlueprint(glm::vec2 size) {
    m_MapGridSize = size;

    m_MapGrid = std::vector<std::shared_ptr<RoomInfo>>(
        size.x * size.y, nullptr
    );
}

std::shared_ptr<RoomInfo> MapBlueprint::GetElementByCooridinate(glm::vec2 pos) {
    if (!this->isElementInBound(pos)) {
        throw std::runtime_error("Position is out of range");
    }

    int index = m_MapGridSize.x * pos.x + pos.y;

    return this->m_MapGrid[index];
}

bool MapBlueprint::isElementInBound(glm::vec2 pos) {
    if (pos.x >= this->m_MapGridSize.x) {
        return false;
    }

    if (pos.y >= this->m_MapGridSize.y) {
        return false;
    }
    return true;
}

void MapBlueprint::SetElementByCooridinate(glm::vec2 pos, std::shared_ptr<RoomInfo> info) {
    
    if (!this->isElementInBound(pos)) {
        throw std::runtime_error("Position is out of range");
    }

    int index = m_MapGridSize.x * pos.x + pos.y;

    this->m_MapGrid[index] = info;
}