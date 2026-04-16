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
            info,
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
                MapColliderConfig::kDefaultWallThickness,
                info->roomType,
                info->roomPurpose
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
        RoomType::ROOM_13_13,
        RoomPurpose::STARTER
    );

    this->m_Blueprint->SetElementByCooridinate(
        this->GetStarterChamberCooridinate(),
        starterRoom
    );

    this->PopulateRoomContents();
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

glm::vec2 MapGenerator::GetRandomPositionInChamber(RoomType type) {
    int widthRadius, heightRadius;

    switch (type) {
        case RoomType::ROOM_13_13:
            widthRadius = 6;
            heightRadius = 6;
            break;

        case RoomType::ROOM_15_15:
            widthRadius = 7;
            heightRadius = 7;
            break;

        case RoomType::ROOM_17_17:
            widthRadius = 8;
            heightRadius = 8;
            break;

        case RoomType::ROOM_17_23:
            widthRadius = 8;
            heightRadius = 11;
            break;

        case RoomType::ROOM_23_17:
            widthRadius = 11;
            heightRadius = 8;
            break;
        
        default:
            throw std::runtime_error("Invalid room type");
    }

    // Safe margin to prevent entity from stucking in the wall
    widthRadius -= 1;
    heightRadius -= 1;

    return glm::vec2(
        this->m_RandomChoose->GetFloat(-widthRadius*MAP_PIXEL_PER_BLOCK, widthRadius*MAP_PIXEL_PER_BLOCK),
        this->m_RandomChoose->GetFloat(-heightRadius*MAP_PIXEL_PER_BLOCK, heightRadius*MAP_PIXEL_PER_BLOCK)
    );
} 

void MapGenerator::PopulateRoomContents() {
    // 遍歷所有已生成的房間
    for (glm::ivec2 coord : this->m_Blueprint->GetAllChamberCooirdinate()) {
        auto info = this->m_Blueprint->GetElementByCooridinate(coord);
        if (info == nullptr) continue;

        // 只針對戰鬥房生成波數
        if (info->roomPurpose == RoomPurpose::FIGHTING) {
            // 先隨機生成 0 到 4 個箱子障礙物，才能知道怪物該避開哪裡
            int boxCount = this->m_RandomChoose->GetInteger(0, 4);
            
            for (int i = 0; i < boxCount; ++i) {
                glm::vec2 pos = this->GetRandomPositionInChamber(info->roomType);
                info->obstacles.push_back({static_cast<int>(ObstacleType::WOODEN_BOX), pos});
            }

            int waveCount = this->m_RandomChoose->GetInteger(1, 3); // 隨機 1 到 3 波
            const float safeDistance = 60.0F; // 怪物與箱子之間的最短安全距離 (可依照你的素材大小調整)
            
            for (int i = 0; i < waveCount; ++i) {
                std::vector<SpawnInfo> wave;
                int monsterCount = this->m_RandomChoose->GetInteger(2, 5); // 每波 2 到 5 隻

                for (int j = 0; j < monsterCount; ++j) {
                    glm::vec2 spawnPos;
                    bool isValid = false;

                    // 嘗試產生相對於房間中心的隨機位置，並檢查是否與箱子重疊 (最多嘗試 10 次避免無窮迴圈)
                    for (int attempt = 0; attempt < 10; ++attempt) {
                        spawnPos = this->GetRandomPositionInChamber(info->roomType);
                        
                        isValid = true;
                        for (const auto& obs : info->obstacles) {
                            if (glm::distance(spawnPos, obs.localPosition) < safeDistance) {
                                isValid = false;
                                break;
                            }
                        }
                        
                        if (isValid) {
                            break; // 找到沒和箱子重疊的位置
                        }
                    }
                    
                    wave.push_back({static_cast<int>(MonsterType::GOBLIN_GUARD), spawnPos});
                }
                
                info->monsterWaves.push_back(MonsterWave(wave));
            }
        }
    }
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
