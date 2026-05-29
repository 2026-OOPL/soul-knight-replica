#include <cmath>
#include <vector>
#include <glm/vec2.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Common/Constants.hpp"
#include "Common/Enums.hpp"
#include "Common/Random.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/BossRoom.hpp"
#include "Component/Map/FightRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Map/MapColliderConfig.hpp"
#include "Component/Map/PortalRoom.hpp"
#include "Component/Map/RewardRoom.hpp"
#include "Component/Map/StarterRoom.hpp"
#include "Generator/GenBossChamber.hpp"
#include "Generator/GenFightChamber.hpp"
#include "Generator/GenPortalChamber.hpp"
#include "Generator/GenRewardChamber.hpp"
#include "Generator/RoomInfo.hpp"
#include "Generator/MapGenerator.hpp"
#include "Util/Logger.hpp"

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

    case RoomPurpose::BOSS:
        return std::make_shared<BossRoom>(
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
        return std::make_shared<RewardRoom>(
            absolutePosition,
            doorConfig,
            info,
            MapColliderConfig::kDefaultWallThickness
        );
    }

    return nullptr;
}

std::string MakeCoordinateKey(const glm::ivec2 &coordinate) {
    return std::to_string(coordinate.x) + ":" + std::to_string(coordinate.y);
}

std::shared_ptr<Gangway> BuildGangway(
    const std::shared_ptr<BaseRoom> &firstRoom,
    DoorSide firstSide,
    const std::shared_ptr<BaseRoom> &secondRoom,
    DoorSide secondSide
) {
    if (firstRoom == nullptr || secondRoom == nullptr) {
        return nullptr;
    }

    if (!firstRoom->HasPassageOnSide(firstSide) || !secondRoom->HasPassageOnSide(secondSide)) {
        return nullptr;
    }

    const BaseRoom::PassageSocket firstSocket = firstRoom->GetPassageSocket(firstSide);
    const BaseRoom::PassageSocket secondSocket = secondRoom->GetPassageSocket(secondSide);
    Gangway::Config config;
    config.width = std::max(
        0.0F,
        std::min(firstSocket.openingSize, secondSocket.openingSize)
    );

    if (firstSide == DoorSide::Left || firstSide == DoorSide::Right) {
        config.orientation = GangwayOrientation::Horizontal;
        config.length = std::abs(secondSocket.worldCenter.x - firstSocket.worldCenter.x);
    } else {
        config.orientation = GangwayOrientation::Vertical;
        config.length = std::abs(secondSocket.worldCenter.y - firstSocket.worldCenter.y);
    }

    glm::vec2 center = (firstSocket.worldCenter + secondSocket.worldCenter) / 2.0F;
    
    if (config.orientation == GangwayOrientation::Horizontal) {
        int firstRoomSize = firstRoom->GetAreaSize().x / MAP_PIXEL_PER_BLOCK;
        int secondRoomSize = secondRoom->GetAreaSize().x / MAP_PIXEL_PER_BLOCK;

        // 計算尺寸差值
        int diff = std::abs(firstRoomSize - secondRoomSize);
        // 當差值除以 2 為奇數時，代表中心點落在了整數格，需要偏移半格 (8px) 來對齊
        float offset = ((diff / 2) % 2 == 0) ? 8.0F : 0.0F;
        center.x += offset;
    } else {
        int firstRoomSize = firstRoom->GetAreaSize().y / MAP_PIXEL_PER_BLOCK;
        int secondRoomSize = secondRoom->GetAreaSize().y / MAP_PIXEL_PER_BLOCK;

        int diff = std::abs(firstRoomSize - secondRoomSize);
        float offset = ((diff / 2) % 2 == 0) ? 8.0F : 0.0F;
        center.y += offset;
    }

    const std::shared_ptr<Gangway> gangway = std::make_shared<Gangway>(center, config);
    gangway->ConnectRooms(firstRoom, secondRoom);
    return gangway;
}

} // namespace

MapGenerator::MapGenerator(GeneratorType type)
: MapGenerator(type, std::make_shared<RandomChoose>()) {}

MapGenerator::MapGenerator(GeneratorType type, std::string seed) 
: MapGenerator(type, std::make_shared<RandomChoose>(seed)) {}

MapGenerator::MapGenerator(GeneratorType type, std::shared_ptr<RandomChoose> random) {
    this->m_RandomChoose = random;

    const int mapSize = m_RandomChoose->GetInteger(5, 10);

    this->m_Blueprint = std::make_shared<MapBlueprint>(glm::vec2(mapSize, mapSize));
    this->m_MapGridSize = m_Blueprint->GetSize();
    this->m_StartDirection = m_RandomChoose->GetEnum<Direction>();
    this->m_StartCoordinateOffset = m_RandomChoose->GetInteger(1, mapSize - 1);
    this->m_StartChamberCooridinate = this->GetStarterChamberCooridinate();

    switch (type) {
        case GeneratorType::EASY:
            this->m_MinimumFightRoomCount = 2;
            this->m_MaximumFightRoomCount = 2;
            this->m_MinimumRewardRoomCount = 1;
            this->m_MaximumRewardRoomCount = 1;
            break;

        case GeneratorType::MEDIUM:
            this->m_MinimumFightRoomCount = 3;
            this->m_MaximumFightRoomCount = 3;
            this->m_MinimumRewardRoomCount = 1;
            this->m_MaximumRewardRoomCount = 1;
            break;
        case GeneratorType::HARD:
        case GeneratorType::BOSS_1:
        case GeneratorType::BOSS_2:
        case GeneratorType::BOSS_3:
            this->m_MinimumFightRoomCount = 4;
            this->m_MaximumFightRoomCount = 4;
            this->m_MinimumRewardRoomCount = 1;
            this->m_MaximumRewardRoomCount = 1;
            break;
    }
    
    const std::shared_ptr<RoomInfo> starterRoom = std::make_shared<RoomInfo>(
        RoomType::ROOM_13_13, RoomPurpose::STARTER, m_RandomChoose
    );
    this->m_Blueprint->SetElementByCooridinate(
        this->m_StartChamberCooridinate,
        starterRoom
    );

    m_GenChamber.push_back(
        std::make_shared<GenFightChamber>(
            this->GetFightingChamberStartCooridinate(),
            [this](glm::vec2 p) { return this->FightChamberCooridinateValidator(p); },
            m_MaximumFightRoomCount,
            m_MinimumFightRoomCount,
            this->m_Blueprint,
            m_RandomChoose
        )
    );

    m_GenChamber.push_back(
        std::make_shared<GenRewardChamber>(
            [this](glm::vec2 p) { return this->RewardChamberCooridinateValidator(p); },
            m_MaximumRewardRoomCount,
            m_MinimumRewardRoomCount,
            this->m_Blueprint,
            m_RandomChoose
        )
    );

    switch (type) {
        case GeneratorType::BOSS_1:
        case GeneratorType::BOSS_2:
        case GeneratorType::BOSS_3:
            m_GenChamber.push_back(
                std::make_shared<GenBossChamber>(
                    [this](glm::vec2 p) { return this->RewardChamberCooridinateValidator(p); },
                    this->m_Blueprint,
                    m_RandomChoose
                )
            );
        default:
            break;
    }

    m_GenChamber.push_back(
        std::make_shared<GenPortalChamber>(
            this->GetPortalChamberGenPolicy(),
            [this](glm::ivec2 coor) {
                return this->PortalChamberCooridinateValidator(coor);
            },
            this->m_Blueprint,
            m_RandomChoose
        )
    );

    int retries = 0;
    bool generateSuccess = false;
    do {
        try {
            for (auto const &i : this->m_GenChamber) i->Generate();
        } catch (const std::runtime_error& _) {
            retries ++;
            generateSuccess = false;
            this->m_Blueprint->Reset();
            this->m_MapGridSize = m_Blueprint->GetSize();
            LOG_INFO("Retrying generating map... ");
            continue;
        }

        generateSuccess = true;
    } while (!generateSuccess || retries > 10);

    if (!generateSuccess) {
        LOG_ERROR("Failed to generate map after 10 retries");
        throw std::runtime_error("Failed to generate map after 10 retries");
    }

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

glm::ivec2 MapGenerator::GetStarterChamberCooridinate() {
    switch (m_StartDirection) {
    case Direction::BOTTOM:
        return glm::ivec2(m_StartCoordinateOffset, m_MapGridSize.y - 1);

    case Direction::LEFT:
        return glm::ivec2(m_StartCoordinateOffset, 0);

    case Direction::RIGHT:
        return glm::ivec2(m_MapGridSize.x - 1, m_StartCoordinateOffset);

    case Direction::TOP:
        return glm::ivec2(m_StartCoordinateOffset, 0);
    }

    return glm::ivec2(0, 0);
}

glm::ivec2 MapGenerator::GetFightingChamberStartCooridinate() {
    const glm::ivec2 startCoridinate = this->GetStarterChamberCooridinate();

    switch (m_StartDirection) {
    case Direction::BOTTOM:
        return startCoridinate + glm::ivec2(0, -1);

    case Direction::LEFT:
        return startCoridinate + glm::ivec2(1, 0);

    case Direction::RIGHT:
        return startCoridinate + glm::ivec2(-1, 0);

    case Direction::TOP:
        return startCoridinate + glm::ivec2(0, 1);
    }

    return glm::ivec2(0, 0);
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
                    BuildGangway(sourceRoom, DoorSide::Right, targetRoom, DoorSide::Left);
                if (gangway != nullptr) {
                    this->m_GangwayInstances.push_back(gangway);
                }
            }

            const glm::ivec2 topCoordinate = chamberCoordinate + glm::ivec2(0, 1);
            const auto topIt = roomsByCoordinate.find(MakeCoordinateKey(topCoordinate));
            if (topIt != roomsByCoordinate.end()) {
                const std::shared_ptr<BaseRoom> targetRoom = topIt->second;
                const std::shared_ptr<Gangway> gangway =
                    BuildGangway(sourceRoom, DoorSide::Top, targetRoom, DoorSide::Bottom);
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

std::vector<std::shared_ptr<BaseRoom>> MapGenerator::GetRooms(RoomPurpose type) {
    std::vector<std::shared_ptr<BaseRoom>> result;

    this->BuildRuntimeMap();

    for (const auto &room : this->m_RoomInstances) {
        if (room == nullptr || room->GetPurpose() != type) {
            continue;
        }

        result.push_back(room);
    }

    return result;
}

std::vector<std::shared_ptr<Gangway>> MapGenerator::GetGangways() {
    this->BuildRuntimeMap();
    return this->m_GangwayInstances;
}

unsigned int long long MapGenerator::GetSeed() {
    return this->m_RandomChoose->GetSeed();
}

bool MapGenerator::PortalChamberCooridinateValidator(glm::ivec2 coor) {
    // 1. 確保傳送門絕對不會選中起點房間座標本身
    if (coor == this->GetStarterChamberCooridinate()) {
        return false;
    }
    // 2. 確保傳送門絕對不會緊鄰起點房間
    glm::ivec2 starterCoor = this->GetStarterChamberCooridinate();
    const glm::ivec2 directions[] = {
        glm::ivec2(0, 1),
        glm::ivec2(0, -1),
        glm::ivec2(1, 0),
        glm::ivec2(-1, 0),
    };
    for (int i = 0; i < 4; ++i) {
        if (coor == starterCoor + directions[i]) {
            return false;
        }
    }
    
    // 3. 尋找 Boss 房座標
    std::vector<glm::ivec2> rooms = m_Blueprint->GetChamberCooirdinateByPurpose(RoomPurpose::BOSS);
    if (rooms.empty()) {
        return true;
    }

    glm::ivec2 bossCoordinate = rooms.front();

    // 4. 若為 Boss 地圖，套用 Boss 專用限制
    bool adjacentToBoss = false;
    for (int i = 0; i < 4; ++i) {
      if (coor == bossCoordinate + directions[i]) {
        adjacentToBoss = true;
        break;
      }
    }

    if (!adjacentToBoss) {
      return false;
    }

    // 統計該座標相鄰的已生成房間數量
    auto getNeighborChamberCount = [this, directions](glm::ivec2 checkCoor) {
      int count = 0;
      for (int i = 0; i < 4; ++i) {
        glm::ivec2 neighbor = checkCoor + directions[i];
        if (this->m_Blueprint->isCooridinateInBound(neighbor) &&
            this->m_Blueprint->GetElementByCooridinate(neighbor) != nullptr) {
          count++;
        }
      }
      return count;
    };

    bool existsExclusiveNeighbor = false;
    for (int i = 0; i < 4; ++i) {
      glm::ivec2 neighbor = bossCoordinate + directions[i];
      if (this->m_Blueprint->isCooridinateInBound(neighbor) &&
          this->m_Blueprint->GetElementByCooridinate(neighbor) == nullptr) {
        if (getNeighborChamberCount(neighbor) == 1) {
          existsExclusiveNeighbor = true;
          break;
        }
      }
    }

    int myNeighbors = getNeighborChamberCount(coor);
    if (existsExclusiveNeighbor) {
      return myNeighbors == 1;
    }
    
    return true;
}