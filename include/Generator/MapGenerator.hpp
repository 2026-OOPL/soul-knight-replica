#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "MapBlueprint.hpp"
#include "Component/Map/BaseRoom.hpp"

class GenChamber;
class Gangway;
class RandomChoose;

enum class Direction;
enum class GeneratePolicy;

enum class GeneratorType {
    EASY,
    MEDIUM,
    HARD,
    BOSS_1,
    BOSS_2,
    BOSS_3
};

class MapGenerator {
public:
    MapGenerator(GeneratorType type);
    MapGenerator(GeneratorType type, std::string seed);
    MapGenerator(GeneratorType type, std::shared_ptr<RandomChoose> random);

    std::vector<std::shared_ptr<BaseRoom>> GetRooms();
    std::vector<std::shared_ptr<BaseRoom>> GetRooms(RoomPurpose type);

    std::vector<std::shared_ptr<Gangway>> GetGangways();

    unsigned int long long GetSeed();

    std::shared_ptr<MapBlueprint> m_Blueprint;

protected:
    glm::ivec2 GetStarterChamberCooridinate();
    glm::ivec2 GetFightingChamberStartCooridinate();

    GeneratePolicy GetPortalChamberGenPolicy();

    std::shared_ptr<RandomChoose> m_RandomChoose;

    bool FightChamberCooridinateValidator(glm::ivec2 cooridinate);
    bool RewardChamberCooridinateValidator(glm::ivec2 cooridinate);
    bool PortalChamberCooridinateValidator(glm::ivec2 cooridinate);

    void BuildRuntimeMap();

private:
    std::vector<std::shared_ptr<GenChamber>> m_GenChamber;

    glm::ivec2 m_MapGridSize;
    glm::ivec2 m_CurrentPosition;

    glm::ivec2 m_StartChamberCooridinate;

    Direction m_StartDirection;
    int m_StartCoordinateOffset;

    bool m_RuntimeMapBuilt = false;
    std::vector<std::shared_ptr<BaseRoom>> m_RoomInstances;
    std::vector<std::shared_ptr<Gangway>> m_GangwayInstances;

    int m_MinimumFightRoomCount;
    int m_MaximumFightRoomCount;
    int m_MinimumRewardRoomCount;
    int m_MaximumRewardRoomCount;
    
    GeneratorType m_GenerateType;
};

#endif
