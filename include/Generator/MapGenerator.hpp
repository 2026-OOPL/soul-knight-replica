#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "../Common/Random.hpp"
#include "../Common/EnumMask.hpp"

struct RoomInfo {
    RoomInfo (RoomType type) {
        roomType = type;
    }
    
    // Which of the size is used for this room  
    RoomType roomType;
};

class MapGenerator {
public:
    MapGenerator(std::string seed) ;
    
    void GenerateRoom();

protected:
    std::shared_ptr<RoomInfo> GetInfoByCooridinate(glm::vec2 coord);

    glm::vec2 GetStartCooridinate();

    glm::vec2 GetRandomNeighbor();
    
private:
glm::vec2 m_CurrentPosition;

int m_MapGridSize;    
int m_RemainRoomCount;

int m_FightingChamberCount;
    int m_RewardChamberCount;
    int m_PortalChamberCount;

    std::shared_ptr<RandomChoose> m_RandomChoose;
    std::vector<std::shared_ptr<RoomInfo>> m_MapGrid;
    
    void GenerateRoom(glm::vec2 coord);

    void GenerateFightingChamber();
};

#endif