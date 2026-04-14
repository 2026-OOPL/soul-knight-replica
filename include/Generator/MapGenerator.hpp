#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "MapBlueprint.hpp"
#include "Component/Map/BaseRoom.hpp"

class GenChamber;
class Gangway;
class RandomChoose;

enum class Direction;
enum class GeneratePolicy;


class MapGenerator {
public:
    MapGenerator(std::string seed) ;
    
    void Generate();

    std::vector<std::shared_ptr<BaseRoom>> GetRooms();
    std::vector<std::shared_ptr<Gangway>> GetGangways();

    std::shared_ptr<MapBlueprint> m_Blueprint;

protected:
    glm::vec2 GetStarterChamberCooridinate();
    glm::vec2 GetFightingChamberStartCooridinate();
    GeneratePolicy GetPortalChamberGenPolicy();

    std::shared_ptr<GenChamber> m_GenChamber;
    std::shared_ptr<RandomChoose> m_RandomChoose;

    bool FightChamberCooridinateValidator(glm::ivec2 cooridinate);
    bool RewardChamberCooridinateValidator(glm::ivec2 cooridinate);
    void BuildRuntimeMap();

private:
    glm::ivec2 m_MapGridSize;
    glm::ivec2 m_CurrentPosition;

    glm::ivec2 m_StartChamberCooridinate;

    Direction m_StartDirection;
    int m_StartCoordinateOffset;

    bool m_RuntimeMapBuilt = false;
    std::vector<std::shared_ptr<BaseRoom>> m_RoomInstances;
    std::vector<std::shared_ptr<Gangway>> m_GangwayInstances;
};

#endif
