#ifndef ROOM_ASSEMBLY_HPP
#define ROOM_ASSEMBLY_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/MapPiece.hpp"

class RoomAssembly {
public:
    explicit RoomAssembly(RoomAssemblyConfig config);

    const std::shared_ptr<BaseRoom> &GetRoom() const;
    const std::vector<std::shared_ptr<Door>> &GetDoors() const;
    const std::vector<std::shared_ptr<MapPiece>> &GetPieces() const;

    glm::vec2 GetSuggestedBottomSpawn(float distanceFromRoom) const;
    RoomPurpose GetPurpose() const;

    void CloseAllDoors();

private:
    RoomAssemblyConfig m_Config;
    std::shared_ptr<BaseRoom> m_Room;
    std::vector<std::shared_ptr<Door>> m_Doors;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;
};

#endif
