#ifndef MAP_SYSTEM_HPP
#define MAP_SYSTEM_HPP

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Component/Camera/Camera.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Mob/Mob.hpp"
#include "Component/Player/Player.hpp"
#include "Scene.hpp"

class MapSystem : public Scene {
public:
    enum class DoorPassageState {
        Idle,
        Traversing
    };

    MapSystem();

    ~MapSystem() override = default;

    bool IsPlayerInsideRoom() const;
    glm::vec2 GetCameraCoor() const;

    void AddRoom(const std::shared_ptr<BaseRoom> &room);
    void AddRooms(const std::vector<std::shared_ptr<BaseRoom>> &rooms);
    void AddGangway(const std::shared_ptr<Gangway> &gangway);
    void AddGangways(const std::vector<std::shared_ptr<Gangway>> &gangways);

    Collision::MovementResult ResolvePlayerMovement(
        const Collision::AxisAlignedBox &currentBox,
        const glm::vec2 &intendedDelta
    );

protected:
    struct DoorPassageContext {
        DoorPassageState state = DoorPassageState::Idle;
        std::shared_ptr<BaseRoom> sourceRoom = nullptr;
        std::shared_ptr<BaseRoom> targetRoom = nullptr;
        DoorSide targetEntrySide = DoorSide::Bottom;
    };

    std::vector<Collision::AxisAlignedBox> CollectCurrentRoomColliders() const;
    std::vector<Collision::AxisAlignedBox> CollectRoomColliders(
        const std::shared_ptr<BaseRoom> &room,
        const Collision::AxisAlignedBox *playerBox
    ) const;
    bool HasRoomPassageBetween(
        const std::shared_ptr<BaseRoom> &sourceRoom,
        const std::shared_ptr<BaseRoom> &targetRoom,
        DoorSide &targetEntrySide
    ) const;
    bool TryStartDoorPassage(const std::shared_ptr<BaseRoom> &targetRoom);
    bool HasCommittedDoorPassage(const glm::vec2 &playerPos) const;
    void CommitDoorPassage();
    void CancelDoorPassage();
    void PrepareDoorPassage(const glm::vec2 &playerPos);
    std::shared_ptr<BaseRoom> FindRoomByPlayerPosition(const glm::vec2 &playerPos) const;
    void UpdateCurrentRoom(const glm::vec2 &playerPos);

    Collision::CollisionSystem m_CollisionSystem;
    std::vector<std::shared_ptr<Mob>> m_Mobs;
    std::vector<std::shared_ptr<Player>> m_Players;
    std::vector<std::shared_ptr<Camera>> m_Cameras;
    std::vector<std::shared_ptr<BaseRoom>> m_Rooms;
    std::vector<std::shared_ptr<Gangway>> m_Gangways;
    std::shared_ptr<BaseRoom> m_CurrentRoom;
    DoorPassageContext m_DoorPassage;
    std::shared_ptr<Camera> m_AttachCamera;
};

#endif
