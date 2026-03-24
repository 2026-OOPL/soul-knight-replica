#include <memory>
#include <vector>
#include <stdexcept>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Common/Constants.hpp"
#include "Generator/MapGenerator.hpp"

MapGenerator::MapGenerator(std::string seed) {
    m_RandomChoose = std::make_shared<RandomChoose>(seed);

    m_MapGridSize = m_RandomChoose->GetInteger(5, 10);

    // Set random room count from seed 
    m_RemainRoomCount = m_RandomChoose->GetInteger(
        MAP_MINUMUM_ROOM_GENERATE_COUNT,
        MAP_MAXIMUM_ROOM_GENERATE_COUNT
    );

    m_MapGrid = std::vector<std::shared_ptr<RoomInfo>>(
        m_MapGridSize * m_MapGridSize,
        nullptr
    );

    // 1. Generate fighting chamber 
    
    // 2. Generate reward rooms 

    // 3. Generate portal room
}

void MapGenerator::GenerateRoom() {
    m_CurrentPosition = this->GetStartCooridinate();

    glm::vec2 startCooridinate = this->GetStartCooridinate();

    m_CurrentPosition = startCooridinate;

    do {
        this->GenerateRoom(startCooridinate);

        // Hop to the spot which is an empty room
        do {
            m_CurrentPosition = this->GetRandomNeighbor();
        } while (this->GetInfoByCooridinate(m_CurrentPosition) != nullptr);

    } while (1);
}

glm::vec2 MapGenerator::GetRandomNeighbor() {
    int offset = this->m_RandomChoose->GetBool() ? 1 : -1;

    glm::vec2 next;
    
    if (this->m_RandomChoose->GetBool()) {
        next = m_CurrentPosition + glm::vec2(offset, 0);
    } else {
        next = m_CurrentPosition + glm::vec2(0, offset);
    }

    if (next.x >= m_MapGridSize || next.x < 0) {
        return this->GetRandomNeighbor();
    }

    if (next.y >= m_MapGridSize || next.y < 0) {
        return this->GetRandomNeighbor();
    }

    return next;
}

void MapGenerator::GenerateRoom(glm::vec2 posotion) {
    if (this->GetInfoByCooridinate(posotion) != nullptr) {
        throw std::runtime_error("This position already has a room");
    }

}

std::shared_ptr<RoomInfo> MapGenerator::GetInfoByCooridinate(glm::vec2 coord) {
    return this->m_MapGrid[m_MapGridSize * coord.x + coord.y];
}

glm::vec2 MapGenerator::GetStartCooridinate() {
    std::vector<glm::vec2> locations = {
        glm::vec2(0, 1),
        glm::vec2(1, m_MapGridSize-1),
        glm::vec2(m_MapGridSize-1, m_MapGridSize-2),
        glm::vec2(m_MapGridSize-1, 1) 
    };

    int index = m_RandomChoose->GetInteger(0, 3);
    return locations[index];
}

void MapGenerator::GenerateFightingChamber() {
    
    return;
}

