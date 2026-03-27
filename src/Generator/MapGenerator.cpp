#include <memory>
#include <vector>
#include <stdexcept>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Common/Constants.hpp"
#include "Common/Random.hpp"
#include "Generator/GenFightChamber.hpp"
#include "Generator/GenPortalChamber.hpp"
#include "Generator/GenRewardChamber.hpp"
#include "Util/Logger.hpp"
#include "Generator/MapGenerator.hpp"

#include "Component/Map/RoomAssembly.hpp"


MapGenerator::MapGenerator(std::string seed) {
    this->m_RandomChoose = std::make_shared<RandomChoose>(seed);

    int mapSize = m_RandomChoose->GetInteger(5, 10);

    this->m_Blueprint = std::make_shared<MapBlueprint>(glm::vec2(mapSize, mapSize));
    
    this->m_MapGridSize = m_Blueprint->GetSize();
    this->m_StartDirection = m_RandomChoose->GetEnum<Direction>();
    this->m_StartCoordinateOffset = m_RandomChoose->GetInteger(1, mapSize-1);
    
    this->m_StartChamberCooridinate = this->GetStarterChamberCooridinate();
}

bool MapGenerator::FightChamberCooridinateValidator(glm::ivec2 cooridinate) {

    switch (m_StartDirection) {
        case Direction::BOTTOM:
            return (cooridinate.y < m_StartChamberCooridinate.y);

        case Direction::LEFT:
            return (cooridinate.x > m_StartChamberCooridinate.x);

        case Direction::RIGHT:
            return (cooridinate.x > m_StartChamberCooridinate.x);

        case Direction::TOP:
            return (cooridinate.y > m_StartChamberCooridinate.y);
    }

    return true;
}

bool MapGenerator::RewardChamberCooridinateValidator(glm::ivec2 cooridinate) {
    switch (m_StartDirection) {
        case Direction::BOTTOM:
            return (cooridinate.y < m_StartChamberCooridinate.y);

        case Direction::LEFT:
            return (cooridinate.x > m_StartChamberCooridinate.x);

        case Direction::RIGHT:
            return (cooridinate.x > m_StartChamberCooridinate.x);

        case Direction::TOP:
            return (cooridinate.y > m_StartChamberCooridinate.y);
    }
    
    return true;
}

void MapGenerator::Generate() {
    m_GenChamber = std::make_shared<GenFightChamber>(
        this->GetFightingChamberStartCooridinate(),
        [this](glm::vec2 p) {return this->FightChamberCooridinateValidator(p);},
        4,
        2,
        this->m_Blueprint,
        m_RandomChoose
    );

    m_GenChamber->Generate();

    m_GenChamber = std::make_shared<GenRewardChamber>(
        [this](glm::vec2 p) {return this->RewardChamberCooridinateValidator(p);},
        4,
        2,
        this->m_Blueprint,
        m_RandomChoose
    );
    
    m_GenChamber->Generate();

    m_GenChamber = std::make_shared<GenPortalChamber>(
        this->GetPortalChamberGenPolicy(),
        [](glm::vec2 _) {return true;},
        this->m_Blueprint,
        m_RandomChoose
    );
    
    m_GenChamber->Generate();

    std::shared_ptr<RoomInfo> starterRoom = std::make_shared<RoomInfo>(
        RoomType::ROOM_13_13, RoomPurpose::STARTER
    );

    this->m_Blueprint->SetElementByCooridinate(
        this->GetStarterChamberCooridinate(),
        starterRoom
    );
}

glm::vec2 MapGenerator::GetStarterChamberCooridinate() {
    switch (m_StartDirection) {
        case Direction::BOTTOM:
            return glm::vec2(m_StartCoordinateOffset, m_MapGridSize.y-1);
        
        case Direction::LEFT:
            return glm::vec2(m_StartCoordinateOffset, 0);

        case Direction::RIGHT:
            return glm::vec2(m_MapGridSize.x-1, m_StartCoordinateOffset);

        case Direction::TOP:
            return glm::vec2(m_StartCoordinateOffset, 0);
    }

    std::vector<glm::vec2> locations = {
        glm::vec2(0, 1),
        glm::vec2(1, m_MapGridSize.y-1),
        glm::vec2(m_MapGridSize.x-1, m_MapGridSize.y-2),
        glm::vec2(m_MapGridSize.x-1, 1) 
    };

    int index = m_RandomChoose->GetInteger(0, 3);
    return locations[index];
}

glm::vec2 MapGenerator::GetFightingChamberStartCooridinate() {
    glm::vec2 startCoridinate = this->GetStarterChamberCooridinate();

    switch (m_StartDirection) {
        case Direction::BOTTOM:
            return startCoridinate + glm::vec2(0, -1);
        
        case Direction::LEFT:
            return startCoridinate + glm::vec2(1, 0);

        case Direction::RIGHT:
            return startCoridinate + glm::vec2(-1, 0);

        case Direction::TOP:
            return startCoridinate + glm::vec2(0, 1);
    }

    return glm::vec2(0, 0);
}

GeneratePolicy MapGenerator::GetPortalChamberGenPolicy() {
    switch (m_StartDirection) {
        case Direction::BOTTOM:
            if (m_StartCoordinateOffset > m_MapGridSize.x / 2) {
                return GeneratePolicy::TOP_LEFT;
            } else {
                return GeneratePolicy::TOP_RIGHT;
            }

        case Direction::TOP:
            if (m_StartCoordinateOffset > m_MapGridSize.x / 2) {
                return GeneratePolicy::BOTTOM_LEFT;
            } else {
                return GeneratePolicy::BOTTOM_RIGHT;
            }

        case Direction::LEFT:
            if (m_StartCoordinateOffset > m_MapGridSize.y / 2) {
                return GeneratePolicy::TOP_RIGHT;
            } else {
                return GeneratePolicy::BOTTOM_RIGHT;
            }
        
        case Direction::RIGHT:
            if (m_StartCoordinateOffset > m_MapGridSize.y / 2) {
                return GeneratePolicy::TOP_LEFT;
            } else {
                return GeneratePolicy::BOTTOM_LEFT;
            }
    }
    // Random return a value
    return GeneratePolicy::TOP_LEFT;
}

std::vector<RoomAssembly> MapGenerator::GetRoomAssembly() {
    std::vector<RoomAssembly> assembly;

    for (int x=0; x<this->m_MapGridSize.x; x++) {
        for (int y=0; y<this->m_MapGridSize.y; y++) {
            glm::ivec2 chamberCooridinate = glm::ivec2(x, y);
            std::shared_ptr<RoomInfo> info = this->m_Blueprint->GetElementByCooridinate(chamberCooridinate);

            if (info == nullptr) {
                continue;
            }

            glm::ivec2 offset = chamberCooridinate - m_StartChamberCooridinate;

            glm::ivec2 rightChamberCooridinate = chamberCooridinate + glm::ivec2(1, 0);
            // If right has room 
            if (this->m_Blueprint->isCooridinateInBound(rightChamberCooridinate) &&
                this->m_Blueprint->GetElementByCooridinate(rightChamberCooridinate)) {
                // TODO: Generate left gangway
            }

            // If bottom has room
            glm::ivec2 bottomChamberCooridinate = chamberCooridinate + glm::ivec2(0, 1);
            if (this->m_Blueprint->isCooridinateInBound(bottomChamberCooridinate) &&
                this->m_Blueprint->GetElementByCooridinate(bottomChamberCooridinate)) {
                // TODO: Generate bottom gangway
            }

            assembly.push_back(RoomAssembly(
                RoomAssemblyConfig{
                    info->roomType,
                    offset * glm::ivec2(27 * MAP_PIXEL_PER_BLOCK, 27 * MAP_PIXEL_PER_BLOCK)
                }
            ));
        }

    }

    return assembly;
}