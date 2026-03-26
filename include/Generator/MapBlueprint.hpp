#ifndef MAP_BLUEPRINT_HPP
#define MAP_BLUEPRINT_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

struct RoomInfo;
class MapPiece;

class MapBlueprint {
public:
    MapBlueprint(glm::vec2 size);

    std::vector<glm::vec2> GetAllFightChamberCooirdinate();
    
    std::shared_ptr<RoomInfo> GetElementByCooridinate(glm::vec2 pos);
    glm::vec2 GetCooridinateByElement(std::shared_ptr<RoomInfo> element); 

    void SetElementByCooridinate(glm::vec2 pos, std::shared_ptr<RoomInfo> info);

    glm::vec2 GetSize();

    bool isCooridinateInBound(glm::vec2 pos);
    
    std::vector<std::shared_ptr<MapPiece>> GetMapPieces();

private:
    glm::vec2 m_MapGridSize;
    std::vector<std::shared_ptr<RoomInfo>> m_MapGrid;
};

#endif