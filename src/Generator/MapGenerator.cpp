#include <cmath>
#include <exception>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include "Common/Constants.hpp"
#include "Common/Enums.hpp"
#include "Common/Random.hpp"
#include "Component/Map/FightRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Map/GangwayLayoutConfig.hpp"
#include "Component/Map/MapColliderConfig.hpp"
#include "Component/Map/PortalRoom.hpp"
#include "Component/Map/StarterRoom.hpp"
#include "Generator/GenFightChamber.hpp"
#include "Generator/GenPortalChamber.hpp"
#include "Generator/GenRewardChamber.hpp"
#include "Generator/RoomInfo.hpp"
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
    switch (info->GetRoomPurpose()) {
    case RoomPurpose::STARTER:
        return std::make_shared<StarterRoom>(
            absolutePosition,
            doorConfig,
            info->GetRoomType(),
            MapColliderConfig::kDefaultWallThickness
        );

    case RoomPurpose::FIGHTING:
        return std::make_shared<FightRoom>(
            absolutePosition,
            doorConfig,
            info,
            MapColliderConfig::kDefaultWallThickness
        );

    case RoomPurpose::PORTAL:
        return std::make_shared<PortalRoom>(
            absolutePosition,
            doorConfig,
            info,
            MapColliderConfig::kDefaultWallThickness
        );

    case RoomPurpose::REWARD:
        return std::make_shared<BaseRoom>(
            absolutePosition,
            info->GetRoomType(),
            info->GetRoomPurpose(),
            doorConfig,
            BaseRoom::BuildWallConfigFromDoorConfig(
                doorConfig,
                MapColliderConfig::kDefaultWallThickness,
                info->GetRoomType(),
                info->GetRoomPurpose()
            )
        );
    }

    return nullptr;
}

std::string MakeCoordinateKey(const glm::ivec2 &coordinate) {
    return std::to_string(coordinate.x) + ":" + std::to_string(coordinate.y);
}

std::shared_ptr<Gangway> BuildGangway(
    const std::shared_ptr<BaseRoom> &firstRoom,
    const std::shared_ptr<BaseRoom> &secondRoom
) {
    if (firstRoom == nullptr || secondRoom == nullptr) {
        return nullptr;
    }

    Gangway::Config config;
    const glm::vec2 delta =
        secondRoom->GetAbsoluteTranslation() - firstRoom->GetAbsoluteTranslation();

    if (std::abs(delta.x) >= std::abs(delta.y)) {
        config.orientation = GangwayOrientation::Horizontal;
        config.width = MapColliderConfig::kHorizontalDoorOpeningWidth;
        config.positionOffset = GangwayLayoutConfig::kHorizontalPositionOffset;
        config.topWallOffset = GangwayLayoutConfig::kHorizontalTopWallOffset;
        config.rightWallOffset = GangwayLayoutConfig::kHorizontalRightWallOffset;
        config.bottomWallOffset = GangwayLayoutConfig::kHorizontalBottomWallOffset;
        config.leftWallOffset = GangwayLayoutConfig::kHorizontalLeftWallOffset;
        const float gap = std::max(
            0.0F,
            std::abs(delta.x) -
                firstRoom->GetRoomSize().x / 2.0F -
                secondRoom->GetRoomSize().x / 2.0F
        );
        config.length = gap + MapColliderConfig::kDefaultWallThickness;
    } else {
        config.orientation = GangwayOrientation::Vertical;
        config.width = MapColliderConfig::kVerticalDoorOpeningHeight;
        config.positionOffset = GangwayLayoutConfig::kVerticalPositionOffset;
        config.topWallOffset = GangwayLayoutConfig::kVerticalTopWallOffset;
        config.rightWallOffset = GangwayLayoutConfig::kVerticalRightWallOffset;
        config.bottomWallOffset = GangwayLayoutConfig::kVerticalBottomWallOffset;
        config.leftWallOffset = GangwayLayoutConfig::kVerticalLeftWallOffset;
        const float gap = std::max(
            0.0F,
            std::abs(delta.y) -
                firstRoom->GetRoomSize().y / 2.0F -
                secondRoom->GetRoomSize().y / 2.0F  
        );
        config.length = gap + MapColliderConfig::kDefaultWallThickness;
    }
    
    config.wallThickness = MapColliderConfig::kDefaultWallThickness;
    const glm::vec2 center =
        (firstRoom->GetAbsoluteTranslation() + secondRoom->GetAbsoluteTranslation()) / 2.0F;
    const std::shared_ptr<Gangway> gangway = std::make_shared<Gangway>(center, config);
    gangway->ConnectRooms(firstRoom, secondRoom);
    return gangway;
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
    this->m_RuntimeMapBuilt = false;
    this->m_RoomInstances.clear();
    this->m_GangwayInstances.clear();

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
        RoomType::ROOM_13_13, RoomPurpose::STARTER, m_RandomChoose
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

void MapGenerator::BuildRuntimeMap() {
    if (this->m_RuntimeMapBuilt) {
        return;
    }

    this->m_RoomInstances.clear();
    this->m_GangwayInstances.clear();

    std::unordered_map<std::string, std::shared_ptr<BaseRoom>> roomsByCoordinate;

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
            
            if (room == nullptr) {
                continue;
            }

            this->m_RoomInstances.push_back(room);
            roomsByCoordinate.emplace(MakeCoordinateKey(chamberCooridinate), room);
        }
    }

    for (int x = 0; x < this->m_MapGridSize.x; x++) {
        for (int y = 0; y < this->m_MapGridSize.y; y++) {
            const glm::ivec2 chamberCoordinate = {x, y};
            const auto sourceIt = roomsByCoordinate.find(MakeCoordinateKey(chamberCoordinate));
            if (sourceIt == roomsByCoordinate.end()) {
                continue;
            }

            const std::shared_ptr<BaseRoom> sourceRoom = sourceIt->second;

            const glm::ivec2 rightCoordinate = chamberCoordinate + glm::ivec2(1, 0);
            const auto rightIt = roomsByCoordinate.find(MakeCoordinateKey(rightCoordinate));
            if (rightIt != roomsByCoordinate.end()) {
                const std::shared_ptr<BaseRoom> targetRoom = rightIt->second;
                const std::shared_ptr<Gangway> gangway =
                    BuildGangway(sourceRoom, targetRoom);
                if (gangway != nullptr) {
                    this->m_GangwayInstances.push_back(gangway);
                }
            }

            const glm::ivec2 topCoordinate = chamberCoordinate + glm::ivec2(0, 1);
            const auto topIt = roomsByCoordinate.find(MakeCoordinateKey(topCoordinate));
            if (topIt != roomsByCoordinate.end()) {
                const std::shared_ptr<BaseRoom> targetRoom = topIt->second;
                const std::shared_ptr<Gangway> gangway =
                    BuildGangway(sourceRoom, targetRoom);
                if (gangway != nullptr) {
                    this->m_GangwayInstances.push_back(gangway);
                }
            }
        }
    }

    this->m_RuntimeMapBuilt = true;
}

std::vector<std::shared_ptr<BaseRoom>> MapGenerator::GetRooms() {
    this->BuildRuntimeMap();
    return this->m_RoomInstances;
}

std::vector<std::shared_ptr<Gangway>> MapGenerator::GetGangways() {
    this->BuildRuntimeMap();
    return this->m_GangwayInstances;
}
