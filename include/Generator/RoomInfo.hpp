#ifndef ROOM_INFO_HPP
#define ROOM_INFO_HPP

#include "Common/MapObject.hpp"
#include "Common/Random.hpp"
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

enum class RoomType;
enum class RoomPurpose;

enum MobType {
    GOBLIN_GUARD
};

enum ObstacleType {
    WOODEN_BOX
};

template <typename T>
struct SpawnInfo {
    SpawnInfo(T type, glm::vec2 localPosition)
        : type(type), localPosition(localPosition) {

    }

    T type;
    glm::vec2 localPosition;
};

class RoomInfo {
public:
    RoomInfo (
        RoomType type,
        RoomPurpose purpose,
        std::shared_ptr<RandomChoose> random
    ) {
        this->m_Type = type;
        this->m_Purpose = purpose;
        this->m_RandomChoose = random;
    };
    
    RoomType GetRoomType();
    RoomPurpose GetRoomPurpose();
    glm::vec2 GetRandomPositionInChamber();
    glm::ivec2 GetRandomPositionInChamberAligned();

    void AddMonsterWave(std::vector<SpawnInfo<MobType>>& monsterWaves);
    void AddSpawnObject(SpawnInfo<ObstacleType>& spawnObject);

    std::vector<std::vector<SpawnInfo<MobType>>>& GetMonsterWaves();
    std::vector<SpawnInfo<ObstacleType>>& GetObstacle();
     
protected:
    glm::ivec2 GetRoomBlockSize();

    std::vector<std::vector<SpawnInfo<MobType>>> m_MonsterWaves;
    std::vector<SpawnInfo<ObstacleType>> m_SpawnObstacle;

private:
    RoomType m_Type;
    RoomPurpose m_Purpose;

    std::shared_ptr<RandomChoose> m_RandomChoose;
};

#endif