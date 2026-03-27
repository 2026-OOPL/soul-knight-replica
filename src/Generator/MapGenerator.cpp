#include <memory>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "Common/Constants.hpp"
#include "Common/Enums.hpp"
#include "Common/Random.hpp"
#include "Component/Map/FightRoom.hpp"
#include "Component/Map/MapColliderConfig.hpp"
#include "Component/Map/StarterRoom.hpp"
#include "Generator/GenFightChamber.hpp"
#include "Generator/GenPortalChamber.hpp"
#include "Generator/GenRewardChamber.hpp"
#include "Generator/MapGenerator.hpp"

namespace {

bool HasRoomAt(
    const std::shared_ptr<MapBlueprint> &blueprint,
    const glm::ivec2 &coordinate
) {
    return blueprint->isCooridinateInBound(coordinate) &&
           blueprint->GetElementByCooridinate(coordinate) != nullptr;
}

DoorConfig BuildDoorConfig(
    const std::shared_ptr<MapBlueprint> &blueprint,
    const glm::ivec2 &coordinate
) {
    DoorConfig doorConfig;
    doorConfig.top.hasDoor = HasRoomAt(blueprint, coordinate + glm::ivec2(0, 1));
    doorConfig.right.hasDoor = HasRoomAt(blueprint, coordinate + glm::ivec2(1, 0));
    doorConfig.bottom.hasDoor = HasRoomAt(blueprint, coordinate + glm::ivec2(0, -1));
    doorConfig.left.hasDoor = HasRoomAt(blueprint, coordinate + glm::ivec2(-1, 0));
    return doorConfig;
}

std::shared_ptr<BaseRoom> BuildRoom(
    const glm::vec2 &absolutePosition,
    const std::shared_ptr<RoomInfo> &info,
    const DoorConfig &doorConfig
) {
    switch (info->roomPurpose) {
    case RoomPurpose::STARTER:
        return std::make_shared<StarterRoom>(
            absolutePosition,
            doorConfig,
            info->roomType,
            MapColliderConfig::kDefaultWallThickness
        );

    case RoomPurpose::FIGHTING:
        return std::make_shared<FightRoom>(
            absolutePosition,
            doorConfig,
            info->roomType,
            MapColliderConfig::kDefaultWallThickness
        );

    case RoomPurpose::REWARD:
    case RoomPurpose::PORTAL:
        return std::make_shared<BaseRoom>(
            absolutePosition,
            info->roomType,
            info->roomPurpose,
            doorConfig,
            BaseRoom::BuildWallConfigFromDoorConfig(
                doorConfig,
                MapColliderConfig::kDefaultWallThickness
            )
        );
    }

    return nullptr;
}

} // namespace

MapGenerator::MapGenerator(std::string seed) {
    this->m_RandomChoose = std::make_shared<RandomChoose>(seed);

    const int mapSize = m_RandomChoose->GetInteger(5, 10);

    this->m_Blueprint = std::make_shared<MapBlueprint>(glm::vec2(mapSize, mapSize));
    this->m_MapGridSize = m_Blueprint->GetSize();
    this->m_StartDirection = m_RandomChoose->GetEnum<Direction>();
    this->m_StartCoordinateOffset = m_RandomChoose->GetInteger(1, mapSize - 1);

    this->m_StartChamberCooridinate = this->GetStarterChamberCooridinate();
}

bool MapGenerator::FightChamberCooridinateValidator(glm::ivec2 cooridinate) {
    switch (m_StartDirection) {
    case Direction::BOTTOM:
        return cooridinate.y < m_StartChamberCooridinate.y;

    case Direction::LEFT:
        return cooridinate.x > m_StartChamberCooridinate.x;

    case Direction::RIGHT:
        return cooridinate.x > m_StartChamberCooridinate.x;

    case Direction::TOP:
        return cooridinate.y > m_StartChamberCooridinate.y;
    }

    return true;
}

bool MapGenerator::RewardChamberCooridinateValidator(glm::ivec2 cooridinate) {
    switch (m_StartDirection) {
    case Direction::BOTTOM:
        return cooridinate.y < m_StartChamberCooridinate.y;

    case Direction::LEFT:
        return cooridinate.x > m_StartChamberCooridinate.x;

    case Direction::RIGHT:
        return cooridinate.x > m_StartChamberCooridinate.x;

    case Direction::TOP:
        return cooridinate.y > m_StartChamberCooridinate.y;
    }

    return true;
}

void MapGenerator::Generate() {
    m_GenChamber = std::make_shared<GenFightChamber>(
        this->GetFightingChamberStartCooridinate(),
        [this](glm::vec2 p) { return this->FightChamberCooridinateValidator(p); },
        4,
        2,
        this->m_Blueprint,
        m_RandomChoose
    );
    m_GenChamber->Generate();

    m_GenChamber = std::make_shared<GenRewardChamber>(
        [this](glm::vec2 p) { return this->RewardChamberCooridinateValidator(p); },
        4,
        2,
        this->m_Blueprint,
        m_RandomChoose
    );
    m_GenChamber->Generate();

    m_GenChamber = std::make_shared<GenPortalChamber>(
        this->GetPortalChamberGenPolicy(),
        [](glm::vec2) { return true; },
        this->m_Blueprint,
        m_RandomChoose
    );
    m_GenChamber->Generate();

    const std::shared_ptr<RoomInfo> starterRoom = std::make_shared<RoomInfo>(
        RoomType::ROOM_13_13,
        RoomPurpose::STARTER
    );

    this->m_Blueprint->SetElementByCooridinate(
        this->GetStarterChamberCooridinate(),
        starterRoom
    );
}

glm::vec2 MapGenerator::GetStarterChamberCooridinate() {
    switch (m_StartDirection) {
    case Direction::BOTTOM:
        return glm::vec2(m_StartCoordinateOffset, m_MapGridSize.y - 1);

    case Direction::LEFT:
        return glm::vec2(m_StartCoordinateOffset, 0);

    case Direction::RIGHT:
        return glm::vec2(m_MapGridSize.x - 1, m_StartCoordinateOffset);

    case Direction::TOP:
        return glm::vec2(m_StartCoordinateOffset, 0);
    }

    return glm::vec2(0, 0);
}

glm::vec2 MapGenerator::GetFightingChamberStartCooridinate() {
    const glm::vec2 startCoridinate = this->GetStarterChamberCooridinate();

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
        return m_StartCoordinateOffset > m_MapGridSize.x / 2 ?
            GeneratePolicy::TOP_LEFT :
            GeneratePolicy::TOP_RIGHT;

    case Direction::TOP:
        return m_StartCoordinateOffset > m_MapGridSize.x / 2 ?
            GeneratePolicy::BOTTOM_LEFT :
            GeneratePolicy::BOTTOM_RIGHT;

    case Direction::LEFT:
        return m_StartCoordinateOffset > m_MapGridSize.y / 2 ?
            GeneratePolicy::TOP_RIGHT :
            GeneratePolicy::BOTTOM_RIGHT;

    case Direction::RIGHT:
        return m_StartCoordinateOffset > m_MapGridSize.y / 2 ?
            GeneratePolicy::TOP_LEFT :
            GeneratePolicy::BOTTOM_LEFT;
    }

    return GeneratePolicy::TOP_LEFT;
}

std::vector<std::shared_ptr<BaseRoom>> MapGenerator::GetRooms() {
    std::vector<std::shared_ptr<BaseRoom>> rooms;

    for (int x = 0; x < this->m_MapGridSize.x; x++) {
        for (int y = 0; y < this->m_MapGridSize.y; y++) {
            const glm::ivec2 chamberCooridinate = {x, y};
            const std::shared_ptr<RoomInfo> info =
                this->m_Blueprint->GetElementByCooridinate(chamberCooridinate);

            if (info == nullptr) {
                continue;
            }

            const glm::ivec2 offset = chamberCooridinate - m_StartChamberCooridinate;
            const glm::vec2 absolutePosition =
                offset * glm::ivec2(27 * MAP_PIXEL_PER_BLOCK, 27 * MAP_PIXEL_PER_BLOCK);
            const DoorConfig doorConfig = BuildDoorConfig(this->m_Blueprint, chamberCooridinate);

            const std::shared_ptr<BaseRoom> room =
                BuildRoom(absolutePosition, info, doorConfig);
            if (room != nullptr) {
                rooms.push_back(room);
            }
        }
    }

    return rooms;
}
