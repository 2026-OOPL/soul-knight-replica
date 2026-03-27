#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "MapBlueprint.hpp"
#include "Component/Map/RoomAssembly.hpp"

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

    std::vector<std::shared_ptr<RoomAssembly>> GetRoomAssembly();

    std::shared_ptr<MapBlueprint> m_Blueprint;

protected:
    glm::vec2 GetStarterChamberCooridinate();
    glm::vec2 GetFightingChamberStartCooridinate();
    GeneratePolicy GetPortalChamberGenPolicy();

    std::shared_ptr<GenChamber> m_GenChamber;
    std::shared_ptr<RandomChoose> m_RandomChoose;

    bool FightChamberCooridinateValidator(glm::ivec2 cooridinate);
    bool RewardChamberCooridinateValidator(glm::ivec2 cooridinate);

private:
    glm::ivec2 m_MapGridSize;
    glm::ivec2 m_CurrentPosition;

    glm::ivec2 m_StartChamberCooridinate;

    Direction m_StartDirection;
    int m_StartCoordinateOffset;
};

#endif
