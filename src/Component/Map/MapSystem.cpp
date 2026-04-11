#include <cmath>
#include <glm/geometric.hpp>
#include <iterator>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Component/Map/MapSystem.hpp"
#include "Component/Character/Character.hpp"
#include "Component/IStateful.hpp"
#include "Component/Map/FightRoom.hpp"

namespace {

Collision::AxisAlignedBox BuildPrimaryBodyBox(const ICollidable &body) {
    const std::vector<Collision::CollisionPrimitive> primitives =
        Collision::CollisionSystem::BuildCollisionPrimitives(body);
    if (primitives.empty()) {
        return {};
    }

    return primitives.front().box;
}

} // namespace

MapSystem::MapSystem()
    : Scene(),
      m_CollisionQueries(&this->m_CollisionSystem) {
    this->m_World.SetRoot(this);
    this->m_CollisionQueries.SetBlockingPrimitiveProvider(
        [this](const ICollidable *ignoreBody) {
            return this->CollectCurrentRoomCollisionPrimitives(ignoreBody);
        }
    );
    this->m_CollisionSystem.SetBlockingPrimitiveProvider(
        [this]() {
            return this->CollectCurrentRoomCollisionPrimitives();
        }
    );
    this->m_CollisionSystem.SetDynamicBodyProvider(
        [this]() {
            return this->CollectDynamicCollisionBodies();
        }
    );
}

void MapSystem::Update() {
    Scene::Update();

    this->m_CollisionSystem.DispatchCollisions();
    this->PruneDestroyedBullets();

    if (this->m_AttachCamera != nullptr) {
        const std::shared_ptr<IStateful> statefulCamera =
            std::dynamic_pointer_cast<IStateful>(this->m_AttachCamera);

        if (statefulCamera != nullptr) {
            statefulCamera->Update();
        }

        for (const auto &player : this->m_World.GetPlayers()) {
            this->ApplyCameraRecursive(player);
        }
        for (const auto &room : this->m_World.GetRooms()) {
            this->ApplyCameraRecursive(room);
        }
        for (const auto &gangway : this->m_World.GetGangways()) {
            this->ApplyCameraRecursive(gangway);
        }
        for (const auto &mob : this->m_World.GetMobs()) {
            this->ApplyCameraRecursive(mob);
        }
        for (const auto &bullet : this->m_World.GetBullets()) {
            this->ApplyCameraRecursive(bullet);
        }
    }
}

void MapSystem::ApplyCameraRecursive(const std::shared_ptr<Util::GameObject> &object) {
    if (this->m_AttachCamera == nullptr || object == nullptr) {
        return;
    }

    this->m_AttachCamera->SetTransformByCamera(object);

    for (const auto &child : object->GetChildren()) {
        this->ApplyCameraRecursive(child);
    }
}

bool MapSystem::IsPlayerInsideRoom() const {
    if (this->m_World.GetPlayers().empty()) {
        return false;
    }

    std::shared_ptr<BaseRoom> room = this->m_RoomTransitions.GetCurrentRoom();
    if (room == nullptr) {
        room = this->m_RoomTransitions.FindRoomByPlayerPosition(
            this->m_World.GetPlayers().front()->GetAbsoluteTranslation(),
            this->m_World.GetRooms()
        );
    }

    return room != nullptr;
}

glm::vec2 MapSystem::GetCameraCoor() const {
    if (this->m_AttachCamera == nullptr) {
        return {0.0F, 0.0F};
    }

    return this->m_AttachCamera->GetCooridinate();
}

std::shared_ptr<BaseRoom> MapSystem::GetCurrentRoom() const {
    return this->m_RoomTransitions.GetCurrentRoom();
}

void MapSystem::AddRoom(const std::shared_ptr<BaseRoom> &room) {
    if (room == nullptr) {
        return;
    }

    std::shared_ptr<FightRoom> fightRoom = std::dynamic_pointer_cast<FightRoom>(room);
    if (fightRoom != nullptr) {
        // FightRoom requires pointer of map system to spawn waves
        fightRoom->SetMapSystem(this);
    }

    this->m_World.AddRoom(room);
}

void MapSystem::AddRooms(const std::vector<std::shared_ptr<BaseRoom>> &rooms) {
    for (const auto &room : rooms) {
        this->AddRoom(room);
    }
}

void MapSystem::AddGangway(const std::shared_ptr<Gangway> &gangway) {
    if (gangway == nullptr) {
        return;
    }

    this->m_World.AddGangway(gangway);
}

void MapSystem::AddGangways(const std::vector<std::shared_ptr<Gangway>> &gangways) {
    for (const auto &gangway : gangways) {
        this->AddGangway(gangway);
    }
}

void MapSystem::AddPlayer(const std::shared_ptr<Player> &player) {
    if (player == nullptr) {
        return;
    }

    player->SetCollisionResolver(
        [this](const ICollidable &body, const glm::vec2 &intendedDelta) {
            return this->ResolvePlayerMovement(body, intendedDelta);
        }
    );

    this->m_World.AddPlayer(player);

    if (this->m_RoomTransitions.GetCurrentRoom() == nullptr) {
        this->UpdateCurrentRoom(player->GetAbsoluteTranslation());
    }
}

void MapSystem::RemovePlayer(const std::shared_ptr<Player> &player) {
    this->m_World.RemovePlayer(player);
}

const std::vector<std::shared_ptr<Player>> &MapSystem::GetPlayers() const {
    return this->m_World.GetPlayers();
}

Collision::MovementResult MapSystem::ResolvePlayerMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) {
    this->m_RoomTransitions.UpdateCurrentRoom(
        body.GetCollisionOrigin(),
        this->m_World.GetRooms()
    );
    this->m_RoomTransitions.PrepareDoorPassage(
        body.GetCollisionOrigin() + intendedDelta,
        this->m_World.GetRooms()
    );

    if (this->m_RoomTransitions.GetCurrentRoom() == nullptr &&
        !this->m_RoomTransitions.HasActiveDoorPassage()) {
        return {
            intendedDelta,
            false,
            false,
            nullptr,
            nullptr
        };
    }

    const Collision::MovementResult result =
        this->m_CollisionQueries.ResolveMovement(body, intendedDelta);
    this->m_RoomTransitions.UpdateCurrentRoom(
        body.GetCollisionOrigin() + result.resolvedDelta,
        this->m_World.GetRooms()
    );
    return result;
}

Collision::MovementResult MapSystem::ResolveMapMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) const {
    return this->m_CollisionQueries.ResolveMovement(body, intendedDelta);
}

Collision::MovementResult MapSystem::ResolveProjectileMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) const {
    return this->m_CollisionQueries.ResolveMovement(body, intendedDelta);
}

Collision::MovementResult MapSystem::PredictMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) const {
    return this->m_CollisionQueries.PredictMovement(body, intendedDelta);
}

bool MapSystem::CanOccupy(
    const ICollidable &body,
    const glm::vec2 &targetOrigin
) const {
    return this->m_CollisionQueries.CanOccupy(body, targetOrigin);
}

std::vector<Collision::CollisionPrimitive> MapSystem::CollectRoomCollisionPrimitives(
    const std::shared_ptr<BaseRoom> &room,
    const ICollidable *ignoreBody
) const {
    if (room == nullptr) {
        return {};
    }

    const Collision::AxisAlignedBox ignoreBox = ignoreBody == nullptr ?
        Collision::AxisAlignedBox{} :
        BuildPrimaryBodyBox(*ignoreBody);
    const Collision::AxisAlignedBox *ignoreOverlapBox =
        ignoreBody == nullptr ? nullptr : &ignoreBox;
    return room->CollectBlockingPrimitives(ignoreOverlapBox);
}

std::vector<Collision::CollisionPrimitive> MapSystem::CollectCurrentRoomCollisionPrimitives(
    const ICollidable *ignoreBody
) const {
    const std::shared_ptr<BaseRoom> currentRoom = this->m_RoomTransitions.GetCurrentRoom();
    const RoomTransitionSystem::DoorPassageContext &doorPassage =
        this->m_RoomTransitions.GetDoorPassage();

    if (currentRoom == nullptr &&
        doorPassage.state == RoomTransitionSystem::DoorPassageState::Idle) {
        return {};
    }

    std::vector<Collision::CollisionPrimitive> colliders;
    std::unordered_set<const Gangway *> collectedGangways;

    const std::vector<Collision::CollisionPrimitive> currentRoomColliders =
        this->CollectRoomCollisionPrimitives(currentRoom, ignoreBody);
    colliders.insert(
        colliders.end(),
        currentRoomColliders.begin(),
        currentRoomColliders.end()
    );

    if (currentRoom != nullptr) {
        for (const auto &gangway : this->m_World.GetGangways()) {
            if (gangway == nullptr ||
                !gangway->ConnectsRoom(currentRoom) ||
                !collectedGangways.insert(gangway.get()).second) {
                continue;
            }

            const std::vector<Collision::CollisionPrimitive> gangwayPrimitives =
                gangway->CollectBlockingPrimitives();
            colliders.insert(
                colliders.end(),
                gangwayPrimitives.begin(),
                gangwayPrimitives.end()
            );
        }
    }

    if (doorPassage.state == RoomTransitionSystem::DoorPassageState::Traversing &&
        doorPassage.targetRoom != nullptr &&
        doorPassage.targetRoom != currentRoom) {
        const std::vector<Collision::CollisionPrimitive> targetRoomColliders =
            this->CollectRoomCollisionPrimitives(doorPassage.targetRoom, ignoreBody);
        colliders.insert(
            colliders.end(),
            targetRoomColliders.begin(),
            targetRoomColliders.end()
        );

        for (const auto &gangway : this->m_World.GetGangways()) {
            if (gangway == nullptr ||
                !gangway->ConnectsRoom(doorPassage.targetRoom) ||
                !collectedGangways.insert(gangway.get()).second) {
                continue;
            }

            const std::vector<Collision::CollisionPrimitive> gangwayPrimitives =
                gangway->CollectBlockingPrimitives();
            colliders.insert(
                colliders.end(),
                gangwayPrimitives.begin(),
                gangwayPrimitives.end()
            );
        }
    }

    return colliders;
}

std::vector<ICollidable *> MapSystem::CollectDynamicCollisionBodies() const {
    std::vector<ICollidable *> bodies;

    bodies.reserve(
        this->m_World.GetPlayers().size() +
        this->m_World.GetMobs().size() +
        this->m_World.GetBullets().size()
    );

    for (const auto &player : this->m_World.GetPlayers()) {
        if (player != nullptr) {
            bodies.push_back(static_cast<ICollidable *>(player.get()));
        }
    }

    for (const auto &mob : this->m_World.GetMobs()) {
        if (mob != nullptr) {
            bodies.push_back(static_cast<ICollidable *>(mob.get()));
        }
    }

    for (const auto &bullet : this->m_World.GetBullets()) {
        if (bullet != nullptr) {
            bodies.push_back(static_cast<ICollidable *>(bullet.get()));
        }
    }

    return bodies;
}

void MapSystem::UpdateCurrentRoom(const glm::vec2 &playerPos) {
    this->m_RoomTransitions.UpdateCurrentRoom(playerPos, this->m_World.GetRooms());
}

const std::vector<std::shared_ptr<Bullet>>& MapSystem::GetBullets() const {
    return this->m_World.GetBullets();
}

void MapSystem::AddBullet(std::shared_ptr<Bullet> bullet) {
    if (bullet != nullptr) {
        bullet->SetCollisionResolver(
            [this](const ICollidable &body, const glm::vec2 &intendedDelta) {
                return this->ResolveProjectileMovement(body, intendedDelta);
            }
        );
    }

    this->m_World.AddBullet(bullet);
}

void MapSystem::RemoveBullet(std::shared_ptr<Bullet> bullet) {
    this->m_World.RemoveBullet(bullet);
}

void MapSystem::AddMob(std::shared_ptr<Character> mob) {
    if (mob != nullptr) {
        mob->SetCollisionResolver(
            [this](const ICollidable &body, const glm::vec2 &intendedDelta) {
                return this->ResolveMapMovement(body, intendedDelta);
            }
        );
    }

    this->m_World.AddMob(mob);
}

void MapSystem::RemoveMob(std::shared_ptr<Character> mob) {
    this->m_World.RemoveMob(mob);
}

const std::vector<std::shared_ptr<Character>>& MapSystem::GetMob() const {
    return this->m_World.GetMobs();
}

void MapSystem::PruneDestroyedBullets() {
    std::vector<std::shared_ptr<Bullet>> destroyedBullets;

    for (const auto &bullet : this->m_World.GetBullets()) {
        if (bullet != nullptr && bullet->IsDestroyRequested()) {
            destroyedBullets.push_back(bullet);
        }
    }

    for (const auto &bullet : destroyedBullets) {
        this->RemoveBullet(bullet);
    }
}

std::shared_ptr<Character> MapSystem::GetNearestMonster() {
    std::shared_ptr<Character> player = m_World.GetPlayers().front();

    float minDistance = std::numeric_limits<float>::max();
    std::shared_ptr<Character> nearestMonster = nullptr;

    for (auto const &i : m_World.GetMobs()) {
        if (i == nullptr) {
            continue;
        }

        float dist = glm::distance(i->GetAbsoluteTranslation(), player->GetAbsoluteTranslation());
        
        if (dist < minDistance) {
            minDistance = dist;
            nearestMonster = i;
        }
    }

    return nearestMonster;
}