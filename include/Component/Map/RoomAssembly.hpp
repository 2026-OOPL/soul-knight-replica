#ifndef ROOM_ASSEMBLY_HPP
#define ROOM_ASSEMBLY_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/MapPiece.hpp"

class RoomAssembly {
public:
    struct Config {
        glm::vec2 roomCenter = {0.0F, 0.0F};
        float wallThickness = 20.0F;
    };

    explicit RoomAssembly(Config config);

    const std::shared_ptr<BaseRoom> &GetRoom() const;
    const std::vector<std::shared_ptr<Door>> &GetDoors() const; 
    const std::vector<std::shared_ptr<MapPiece>> &GetPieces() const;
    const std::vector<Collision::AxisAlignedBox> &GetStaticWallBoxes() const;

    glm::vec2 GetSuggestedBottomSpawn(float distanceFromRoom) const;

    void CloseAllDoors();

private:
    std::shared_ptr<BaseRoom> m_Room;
    std::vector<std::shared_ptr<Door>> m_Doors;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;
    std::vector<Collision::AxisAlignedBox> m_StaticWallBoxes;
};

#endif
