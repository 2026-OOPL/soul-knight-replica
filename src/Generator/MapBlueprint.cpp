#include <glm/fwd.hpp>
#include <memory>

#include <glm/vec2.hpp>
#include <stdexcept>

#include "Generator/MapBlueprint.hpp"
#include "Common/Enums.hpp"
#include "Generator/MapGenerator.hpp"
#include "Util/Logger.hpp"

MapBlueprint::MapBlueprint(glm::ivec2 size) {
    m_MapGridSize = size;

    m_MapGrid = std::vector<std::shared_ptr<RoomInfo>>(
        size.x * size.y, nullptr
    );
}

std::shared_ptr<RoomInfo> MapBlueprint::GetElementByCooridinate(glm::ivec2 pos) {
    if (!this->isCooridinateInBound(pos)) {
        throw std::runtime_error("Position is out of range");
    }

    int index = pos.y * m_MapGridSize.x + pos.x;

    return this->m_MapGrid[index];
}

bool MapBlueprint::isCooridinateInBound(glm::ivec2 pos) {
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

void MapBlueprint::SetElementByCooridinate(glm::ivec2 pos, std::shared_ptr<RoomInfo> info) {
    
    if (!this->isCooridinateInBound(pos)) {
        throw std::runtime_error("Position is out of range");
    }

    int index = pos.y * m_MapGridSize.x + pos.x;

    this->m_MapGrid[index] = info;
}

glm::ivec2 MapBlueprint::GetSize() {
    return m_MapGridSize;
}

glm::ivec2 MapBlueprint::GetCooridinateByElement(std::shared_ptr<RoomInfo> element) {
    for (int index =0; index < m_MapGridSize.x * m_MapGridSize.y; index++) {
        if (this->m_MapGrid[index] == element) {
            return glm::ivec2(index % m_MapGridSize.x, index / m_MapGridSize.x);
        }
    }

    throw std::runtime_error("Element cannot be found");
}

std::vector<glm::ivec2> MapBlueprint::GetAllFightChamberCooirdinate() {
    std::vector<glm::ivec2> result;
    
    for (auto const& i : m_MapGrid) {
        if (i == nullptr) {
            continue;
        }

        if (i->roomPurpose != RoomPurpose::FIGHTING) {
            continue;
        }

        glm::ivec2 cooridinate = this->GetCooridinateByElement(i);
        result.push_back(cooridinate);
    }

    return result;
}

std::vector<glm::ivec2> MapBlueprint::GetAllChamberCooirdinate() {
    std::vector<glm::ivec2> result;
    
    for (auto const& i : m_MapGrid) {
        if (i == nullptr) {
            continue;
        }

        glm::ivec2 cooridinate = this->GetCooridinateByElement(i);
        result.push_back(cooridinate);
    }

    return result;
}

void MapBlueprint::OutputMapGridType() {
    std::string buf;
    for (int i=0; i<m_MapGridSize.x; i++) {
        for (int j=0; j<m_MapGridSize.y; j++) {
            int index = m_MapGridSize.x * i + j;
            if (this->m_MapGrid[index] == nullptr) {
                buf += "X ";
                continue;
            }
            RoomPurpose purpose = this->m_MapGrid[index]->roomPurpose;

            switch (purpose) {
                case RoomPurpose::FIGHTING:
                    buf += "F";
                    break;

                case RoomPurpose::STARTER:
                    buf += "R";
                    break;

                case RoomPurpose::REWARD:
                    buf += "W";
                    break;

                case RoomPurpose::PORTAL:
                    buf += "P";
                    break;

                default:
                    buf += "?";
                    break;
            }

            buf += " ";
        }
        LOG_DEBUG(buf);
        buf.clear();
    }
}