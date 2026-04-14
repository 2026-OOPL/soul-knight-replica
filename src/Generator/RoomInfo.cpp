
#include "Common/Constants.hpp"
#include "Common/Enums.hpp"
#include <glm/ext/vector_int2.hpp>
#include <stdexcept>
#include <vector>
#include "Generator/RoomInfo.hpp"

glm::ivec2 RoomInfo::GetRoomBlockSize() {
    switch (m_Type) {
        case RoomType::ROOM_13_13:
            return glm::ivec2(13, 13);

        case RoomType::ROOM_15_15:
            return glm::ivec2(15, 15);

        case RoomType::ROOM_17_17:
            return glm::ivec2(17, 17);

        case RoomType::ROOM_17_23:
            return glm::ivec2(17, 23);

        case RoomType::ROOM_23_17:
            return glm::ivec2(23, 17);

        break;
    }

    throw std::runtime_error("Unknown room type");
}

RoomPurpose RoomInfo::GetRoomPurpose() {
    return this->m_Purpose;
}

RoomType RoomInfo::GetRoomType() {
    return this->m_Type;
}

glm::vec2 RoomInfo::GetRandomPositionInChamber() {
    glm::ivec2 sizeInBlocks = this->GetRoomBlockSize();
    int widthRadius = (sizeInBlocks.x - 1) / 2;
    int heightRadius = (sizeInBlocks.y - 1) / 2;

    // Safe margin to prevent entity from stucking in the wall
    widthRadius -= 1;
    heightRadius -= 1;

    return glm::vec2(
        this->m_RandomChoose->GetFloat(-widthRadius*MAP_PIXEL_PER_BLOCK, widthRadius*MAP_PIXEL_PER_BLOCK),
        this->m_RandomChoose->GetFloat(-heightRadius*MAP_PIXEL_PER_BLOCK, heightRadius*MAP_PIXEL_PER_BLOCK)
    );
}

glm::ivec2 RoomInfo::GetRandomPositionInChamberAligned() {
    glm::ivec2 sizeInBlocks = this->GetRoomBlockSize();
    int widthRadius = (sizeInBlocks.x - 1) / 2;
    int heightRadius = (sizeInBlocks.y - 1) / 2;

    // Safe margin to prevent entity from stucking in the wall
    widthRadius -= 1;
    heightRadius -= 1;

    int targetXBlock = -widthRadius + this->m_RandomChoose->GetInteger(sizeInBlocks.x);
    int targetYBlock = -heightRadius + this->m_RandomChoose->GetInteger(sizeInBlocks.y);

    return glm::vec2(
        targetXBlock*MAP_PIXEL_PER_BLOCK,
        targetYBlock*MAP_PIXEL_PER_BLOCK
    );

}

void RoomInfo::AddMonsterWave(std::vector<SpawnInfo<MobType>>& monsterWaves) {
    this->m_MonsterWaves.push_back(monsterWaves);
}

void RoomInfo::AddSpawnObject(SpawnInfo<ObstacleType>& spawnObject) {
    this->m_SpawnObstacle.push_back(spawnObject);
}

std::vector<std::vector<SpawnInfo<MobType>>>& RoomInfo::GetMonsterWaves() {
    return this->m_MonsterWaves;
}

std::vector<SpawnInfo<ObstacleType>>& RoomInfo::GetObstacle() {
    return this->m_SpawnObstacle;
}