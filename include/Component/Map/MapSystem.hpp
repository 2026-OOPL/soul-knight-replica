#ifndef MAP_SYSTEM_HPP
#define MAP_SYSTEM_HPP

#include <memory>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include "Util/GameObject.hpp"

#include "Component/Bullet.hpp"
#include "Component/Camera/Camera.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionQueryService.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Debug/CollisionDebugOverlay.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Map/RoomTransitionSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Component/World/WorldRegistry.hpp"
#include "Scene.hpp"

class MapSystem : public Scene {
public:
    MapSystem();

    ~MapSystem() override = default;

    void Update() override;

    bool IsPlayerInsideRoom() const;
    glm::vec2 GetCameraCoor() const;
    std::shared_ptr<BaseRoom> GetCurrentRoom() const;

    void AddRoom(const std::shared_ptr<BaseRoom> &room);
    void AddRooms(const std::vector<std::shared_ptr<BaseRoom>> &rooms);
    void AddGangway(const std::shared_ptr<Gangway> &gangway);
    void AddGangways(const std::vector<std::shared_ptr<Gangway>> &gangways);
    void AddPlayer(const std::shared_ptr<Player> &player);
    void RemovePlayer(const std::shared_ptr<Player> &player);
    const std::vector<std::shared_ptr<Player>> &GetPlayers() const;

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

    void AddMob(std::shared_ptr<Character> mob);
    void RemoveMob(std::shared_ptr<Character> mob);
    const std::vector<std::shared_ptr<Character>>& GetMob() const;

protected:
    std::vector<Collision::CollisionPrimitive> CollectCurrentRoomCollisionPrimitives(
        const ICollidable *ignoreBody = nullptr
    ) const;
    std::vector<Collision::CollisionPrimitive> CollectRoomCollisionPrimitives(
        const std::shared_ptr<BaseRoom> &room,
        const ICollidable *ignoreBody
    ) const;
    std::vector<ICollidable *> CollectDynamicCollisionBodies() const;
    void UpdateCurrentRoom(const glm::vec2 &playerPos);

    Collision::CollisionSystem m_CollisionSystem;
    CollisionQueryService m_CollisionQueries;
    WorldRegistry m_World;
    RoomTransitionSystem m_RoomTransitions;

    std::vector<std::shared_ptr<Camera>> m_Cameras;

    std::shared_ptr<Camera> m_AttachCamera;

private:
    void ApplyCameraRecursive(const std::shared_ptr<Util::GameObject> &object);
    CollisionDebugCameraState CaptureCollisionDebugCameraState() const;
    std::vector<CollisionDebugVisualEntry> BuildCollisionDebugVisualEntries() const;
    std::vector<CollisionDebugEntry> BuildCollisionDebugEntries() const;
    void PruneDestroyedBullets();
    void PruneDefeatedMobs();

    std::shared_ptr<CollisionDebugOverlay> m_CollisionDebugOverlay;
    bool m_ShowCollisionDebug = false;
};

#endif
