#ifndef MAP_SYSTEM_HPP
#define MAP_SYSTEM_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Prop/Portal.hpp"
#include "Generator/MapGenerator.hpp"
#include "Util/GameObject.hpp"

#include "Component/Bullet.hpp"
#include "Component/Camera/Camera.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionQueryService.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Map/RoomTransitionSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Component/World/WorldRegistry.hpp"
#include "Scene.hpp"

namespace MapSystemConfig {

struct MapConfig {
  int chapter = 1;
  int section = 1;
  GeneratorType difficulty = GeneratorType::EASY;
  std::string seed = "";
};

}

class CollisionDebugOverlay;

class MapSystem : public Scene {
public:
    MapSystem(MapSystemConfig::MapConfig config);

    ~MapSystem() override = default;

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;

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

    void AddMob(std::shared_ptr<Mob> mob);
    void RemoveMob(std::shared_ptr<Mob> mob);
    const std::vector<std::shared_ptr<Mob>>& GetMob() const;

    void AddProp(const std::shared_ptr<Prop> &prop);
    void RemoveProp(const std::shared_ptr<Prop> &prop);
    const std::vector<std::shared_ptr<Prop>> &GetProps() const;

    Collision::CollisionSystem* GetCollisionSystem() { return &this->m_CollisionSystem; }

protected:
    std::vector<Collision::CollisionPrimitive> CollectCollisionPrimitivesForQuery(
        const Collision::AxisAlignedBox &queryBox,
        const ICollidable *ignoreBody = nullptr
    ) const;
    std::vector<Collision::CollisionPrimitive> CollectCurrentRoomCollisionPrimitives(
        const ICollidable *ignoreBody = nullptr
    ) const;
    std::vector<Collision::CollisionPrimitive> CollectRoomCollisionPrimitives(
        const std::shared_ptr<BaseRoom> &room,
        const ICollidable *ignoreBody
    ) const;
    std::vector<Collision::CollisionPrimitive> CollectPropCollisionPrimitives(
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

    std::shared_ptr<Character> GetNearestMonster();

private:
    void ApplyCameraRecursive(const std::shared_ptr<Util::GameObject> &object);
    void DebugClearCurrentFightRoom();
    bool ResolvePlayerMeleeAttack(Player &player);
    void AddBulletImmediately(const std::shared_ptr<Bullet> &bullet);
    void FlushPendingBullets();
    void AddMobImmediately(const std::shared_ptr<Mob> &mob);
    void FlushPendingMobs();
    void PruneDestroyedBullets();
    void PruneDestroyedProps();
    void PruneDefeatedMobs();
    void SpawnDropsForMob(const std::shared_ptr<Mob> &mob);

    std::shared_ptr<CollisionDebugOverlay> m_CollisionDebugOverlay;
    std::vector<std::shared_ptr<Bullet>> m_PendingBullets;
    std::vector<std::shared_ptr<Mob>> m_PendingMobs;
    bool m_IsUpdatingScene = false;
    bool m_ShowCollisionDebug = false;

    std::shared_ptr<Portal> m_CurrentPortal;

    std::shared_ptr<Scene> m_RedirectScene;

    MapSystemConfig::MapConfig m_MapConfig;

};

#endif
