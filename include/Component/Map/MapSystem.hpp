#ifndef MAP_SYSTEM_HPP
#define MAP_SYSTEM_HPP

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Component/Camera/Camera.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Mob/Mob.hpp"
#include "Component/Player/Player.hpp"
#include "Scene.hpp"

class MapSystem : public Scene {
public:
    MapSystem();

    ~MapSystem() override = default;

    bool IsPlayerInsideRoom() const;
    glm::vec2 GetCameraCoor() const;

    void AddRoom(const std::shared_ptr<BaseRoom> &room);
    void AddRooms(const std::vector<std::shared_ptr<BaseRoom>> &rooms);

    Collision::MovementResult ResolvePlayerMovement(
        const Collision::AxisAlignedBox &currentBox,
        const glm::vec2 &intendedDelta
    );

protected:
    std::vector<Collision::AxisAlignedBox> CollectCurrentRoomColliders() const;
    std::shared_ptr<BaseRoom> FindRoomByPlayerPosition(const glm::vec2 &playerPos) const;
    void UpdateCurrentRoom(const glm::vec2 &playerPos);

    Collision::CollisionSystem m_CollisionSystem;
    std::vector<std::shared_ptr<Mob>> m_Mobs;
    std::vector<std::shared_ptr<Player>> m_Players;
    std::vector<std::shared_ptr<Camera>> m_Cameras;
    std::vector<std::shared_ptr<BaseRoom>> m_Rooms;
    std::shared_ptr<BaseRoom> m_CurrentRoom;
    std::shared_ptr<Camera> m_AttachCamera;
};

#endif
