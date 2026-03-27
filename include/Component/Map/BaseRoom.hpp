#ifndef BASE_ROOM_HPP
#define BASE_ROOM_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/MapPiece.hpp"

class BaseRoom : public MapPiece {
public:
    virtual ~BaseRoom() override = default;

    bool IsPlayerInside(const glm::vec2 &playerPos) const;
    Collision::MovementResult ResolvePlayerMovement(
        const Collision::AxisAlignedBox &currentBox,
        const glm::vec2 &intendedDelta
    ) const;

    void SetDoors(const std::vector<std::shared_ptr<Door>> &doors);

    const std::vector<Collision::AxisAlignedBox> &GetWallColliders() const;
    glm::vec2 GetRoomSize() const;
    RoomPurpose GetPurpose() const;

protected:
    BaseRoom(
        const std::string &resourcePath,
        const glm::vec2 &roomSize,
        RoomAssemblyConfig config
    );

private:
    Collision::RoomBoundaryOpenings BuildWallOpenings() const;
    std::vector<Collision::AxisAlignedBox> CollectClosedDoorColliders() const;
    float GetOpeningSizeForSide(DoorSide side) const;
    bool HasOpeningForSide(DoorSide side) const;
    void BuildWallColliders();

    glm::vec2 m_RoomSize = {0.0F, 0.0F};
    RoomAssemblyConfig m_Config;
    std::vector<std::shared_ptr<Door>> m_Doors;
    std::vector<Collision::AxisAlignedBox> m_WallColliders;
    Collision::CollisionSystem m_CollisionSystem;
};

#endif
