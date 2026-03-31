#ifndef MAP_SYSTEM_HPP
#define MAP_SYSTEM_HPP

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "Util/GameObject.hpp"

#include "Component/Bullet.hpp"
#include "Component/Camera/Camera.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Gangway.hpp"
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

    void Update() override;

    bool IsPlayerInsideRoom() const;
    glm::vec2 GetCameraCoor() const;

    void AddRoom(const std::shared_ptr<BaseRoom> &room);
    void AddRooms(const std::vector<std::shared_ptr<BaseRoom>> &rooms);
    void AddGangway(const std::shared_ptr<Gangway> &gangway);
    void AddGangways(const std::vector<std::shared_ptr<Gangway>> &gangways);

    Collision::MovementResult ResolvePlayerMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    );
    Collision::MovementResult ResolveMapMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    ) const;
    Collision::MovementResult ResolveProjectileMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    ) const;
    Collision::MovementResult PredictMovement(
        const ICollidable &body,
        const glm::vec2 &intendedDelta
    ) const;
    bool CanOccupy(
        const ICollidable &body,
        const glm::vec2 &targetOrigin
    ) const;

    void AddBullet(std::shared_ptr<Bullet> bullet);
    void RemoveBullet(std::shared_ptr<Bullet> bullet);
    const std::vector<std::shared_ptr<Bullet>>& GetBullets() const;

    void AddMob(std::shared_ptr<Character> bullet);
    void RemoveMob(std::shared_ptr<Character> bullet);
    const std::vector<std::shared_ptr<Character>>& GetMob() const;

protected:
    struct DoorPassageContext {
        DoorPassageState state = DoorPassageState::Idle;
        std::shared_ptr<BaseRoom> sourceRoom = nullptr;
        std::shared_ptr<BaseRoom> targetRoom = nullptr;
        DoorSide targetEntrySide = DoorSide::Bottom;
    };

    std::vector<Collision::CollisionPrimitive> CollectCurrentRoomCollisionPrimitives(
        const ICollidable *ignoreBody = nullptr
    ) const;
    std::vector<Collision::CollisionPrimitive> CollectRoomCollisionPrimitives(
        const std::shared_ptr<BaseRoom> &room,
        const ICollidable *ignoreBody
    ) const;
    std::vector<ICollidable *> CollectDynamicCollisionBodies() const;
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

    std::vector<std::shared_ptr<Player>> m_Players;
    std::vector<std::shared_ptr<Camera>> m_Cameras;
    std::vector<std::shared_ptr<BaseRoom>> m_Rooms;
    std::vector<std::shared_ptr<Gangway>> m_Gangways;

    std::shared_ptr<BaseRoom> m_CurrentRoom;
    std::shared_ptr<Camera> m_AttachCamera;

    DoorPassageContext m_DoorPassage;

private:
    template <typename T>
    void AddEntity(
        std::shared_ptr<T> entity,
        std::vector<std::shared_ptr<T>>& container,
        const std::string& entityName = "entity"
    ) {
        if (entity == nullptr) {
            throw std::runtime_error("You are trying to add a null " + entityName);
        }
        this->AddChild(entity);
        container.push_back(entity);
    }

    template <typename T>
    void RemoveEntity(
        std::shared_ptr<T> entity,
        std::vector<std::shared_ptr<T>>& container,
        const std::string& entityName = "entity"
    ) {
        if (entity == nullptr) {
            throw std::runtime_error("You are trying to erase a null " + entityName);
        }
        this->RemoveChild(entity);
        container.erase(
            std::remove(container.begin(), container.end(), entity),
            container.end()
        );
    }

    void ApplyCameraRecursive(const std::shared_ptr<Util::GameObject> &object);
    void PruneDestroyedBullets();

    std::vector<std::shared_ptr<Bullet>> m_Bullets;
    std::vector<std::shared_ptr<Character>> m_Mobs;
};

#endif
