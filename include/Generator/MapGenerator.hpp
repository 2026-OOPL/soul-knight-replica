#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP

#include "MapBlueprint.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

class GenChamber;
class RandomChoose;

enum class RoomType;
enum class RoomPurpose;
enum class Direction;
enum class GeneratePolicy;

struct RoomInfo {
public:
    RoomInfo (RoomType type, RoomPurpose roomPurpose) {
        this->roomType = type;
        this->roomPurpose = roomPurpose;
    }
    
    // Which of the size is used for this room
    RoomType roomType;

    RoomPurpose roomPurpose;
};

class MapGenerator {
public:
    MapGenerator(std::string seed) ;
    
    void Generate();

    std::shared_ptr<MapBlueprint> m_Blueprint;

protected:
    glm::vec2 GetStarterChamberCooridinate();
    glm::vec2 GetFightingChamberStartCooridinate();
    GeneratePolicy GetPortalChamberGenPolicy();

    std::shared_ptr<GenChamber> m_GenChamber;
    std::shared_ptr<RandomChoose> m_RandomChoose;

    bool FightChamberCooridinateValidator(glm::vec2 cooridinate);
    bool RewardChamberCooridinateValidator(glm::vec2 cooridinate);

private:
    glm::vec2 m_MapGridSize;
    glm::vec2 m_CurrentPosition;

    glm::vec2 m_StartChamberCooridinate;

    Direction m_StartDirection;
    int m_StartCoordinateOffset;
};

#endif