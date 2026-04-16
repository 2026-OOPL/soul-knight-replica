#ifndef MAP_BLUEPRINT_HPP
#define MAP_BLUEPRINT_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

class MapPiece;
class RoomInfo;

class MapBlueprint {
public:
    MapBlueprint(glm::ivec2 size);

    std::vector<glm::ivec2> GetAllFightChamberCooirdinate();
    std::vector<glm::ivec2> GetAllChamberCooirdinate();
    
    std::shared_ptr<RoomInfo> GetElementByCooridinate(glm::ivec2 pos);
    glm::ivec2 GetCooridinateByElement(std::shared_ptr<RoomInfo> element); 

    void SetElementByCooridinate(glm::ivec2 pos, std::shared_ptr<RoomInfo> info);

    glm::ivec2 GetSize();

    bool isCooridinateInBound(glm::ivec2 pos);

    std::vector<std::shared_ptr<MapPiece>> GetMapPieces();

    void OutputMapGridType();

private:
    glm::ivec2 m_MapGridSize;
    std::vector<std::shared_ptr<RoomInfo>> m_MapGrid;
};

#endif