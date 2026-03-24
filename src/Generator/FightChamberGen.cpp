#include <memory>
#include <stdexcept>
#include <exception>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Generator/MapGenerator.hpp"
#include "Generator/FightChamberGen.hpp"

FightChamberGen::FightChamberGen(
    glm::vec2 start,
    glm::vec2 size,
    glm::vec2 position,
    std::shared_ptr<MapBlueprint> blueprint,
    std::shared_ptr<RandomChoose> random,
    int roomCount
) {
    if (blueprint == nullptr) {
        throw std::runtime_error("Blueprint (which is null) is required for chamber generation");
    }

    try {
        blueprint->GetElementByCooridinate(position + size);
    } catch (std::runtime_error e) {
        throw std::runtime_error("The spicified location is out of the map range");
    }

    if (! this->isCooridinateInBound(start)) {
        throw std::runtime_error("Start position is not in bound");
    }

    if (random == nullptr) {
        throw std::runtime_error("Random chooser is nullptr");
    }

    if (roomCount < 0) {
        throw std::runtime_error("Room count cannot be negative value");
    }

    if (roomCount > size.x * size.y) {
        throw std::runtime_error("Room count is more than the map grid size");
    }
    
    this->m_Start = start;
    this->m_Blueprint = blueprint;
    this->m_MapGridSize = size;
    this->m_MapGridPosition = position;
    this->m_RandomChoose = random;
    this->m_MapRoomCount = roomCount;
}

bool FightChamberGen::isCooridinateInBound(glm::vec2 position) {
    if (position.x < m_MapGridPosition.x ||
        position.y < m_MapGridPosition.y
    ) {
        return false;
    }

    if (position.x > m_MapGridPosition.x + m_MapGridSize.x ||
        position.y > m_MapGridPosition.y + m_MapGridSize.y
    ) {
        return false;
    }

    return true;
}

void FightChamberGen::Generate() {
    glm::vec2 currentPosition = m_Start;

    int i=0;

    do {
        currentPosition = this->GetRandomNeighbor(currentPosition);

        // If the position is out of the generation bound
        if (!this->isCooridinateInBound(currentPosition)) {
            continue;
        }

        // If the position already contains a fighting chamber
        if (this->m_Blueprint->GetElementByCooridinate(currentPosition) != nullptr) {
            continue;
        }
        
        // Random create a fighting chamber size
        this->CreateRoom(currentPosition);

        i++;
    } while (i < m_MapRoomCount);

    
}

void FightChamberGen::CreateRoom(glm::vec2 position) {
    RoomType type = this->m_RandomChoose->GetEnum<RoomType>();

    // Ensure the chamber size is not 13x13
    while (type == RoomType::ROOM_13_13) {
        type = this->m_RandomChoose->GetEnum<RoomType>();
    }

    std::shared_ptr<RoomInfo> info = std::make_shared<RoomInfo>(
        type
    );

    m_Blueprint->SetElementByCooridinate(position, info);
}

glm::vec2 FightChamberGen::GetRandomNeighbor(glm::vec2 current) {
    int offset = this->m_RandomChoose->GetBool() ? 1 : -1;

    glm::vec2 next;
    
    if (this->m_RandomChoose->GetBool()) {
        next = current + glm::vec2(offset, 0);
    } else {
        next = current + glm::vec2(0, offset);
    }

    if (! this->isCooridinateInBound(next)) {
        return this->GetRandomNeighbor(current);
    }

    return next;
}