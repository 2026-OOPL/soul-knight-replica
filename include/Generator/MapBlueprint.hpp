#ifndef MAP_BLUEPRINT_HPP
#define MAP_BLUEPRINT_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "MapGenerator.hpp"

class MapBlueprint {
public:
    MapBlueprint(glm::vec2 size);

    std::vector<std::shared_ptr<RoomInfo>> GetMapGrid();
    
    std::shared_ptr<RoomInfo> GetElementByCooridinate(glm::vec2 pos);
    void SetElementByCooridinate(glm::vec2 pos, std::shared_ptr<RoomInfo> info);

protected:
    bool isElementInBound(glm::vec2 pos);
private:
    glm::vec2 m_MapGridSize;
    std::vector<std::shared_ptr<RoomInfo>> m_MapGrid;
};

#endif