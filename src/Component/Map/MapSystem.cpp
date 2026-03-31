#include <cmath>
#include <unordered_set>

#include "Component/Map/MapSystem.hpp"
#include "Component/IStateful.hpp"

namespace {

constexpr float kDoorPassageCommitDepth = 32.0F;
constexpr float kRoomAlignmentTolerance = 1.0F;

Collision::CollisionFilter BuildWorldBlockingFilter() {
    Collision::CollisionFilter filter;
    filter.layer = Collision::CollisionLayer::World;
    filter.mask = Collision::kAllCollisionLayers;
    filter.blocking = true;
    return filter;
}

Collision::AxisAlignedBox BuildPrimaryBodyBox(const ICollidable &body) {
    const std::vector<Collision::CollisionPrimitive> primitives =
        Collision::CollisionSystem::BuildCollisionPrimitives(body);
    if (primitives.empty()) {
        return {};
    }

    return primitives.front().box;
}

std::vector<Collision::CollisionPrimitive> ToWorldPrimitives(
    const std::vector<Collision::AxisAlignedBox> &boxes
) {
    std::vector<Collision::CollisionPrimitive> primitives;
    const Collision::CollisionFilter worldFilter = BuildWorldBlockingFilter();

    primitives.reserve(boxes.size());
    for (const auto &box : boxes) {
        primitives.push_back(Collision::CollisionSystem::BuildStaticPrimitive(box, worldFilter));
    }

    return primitives;
}

} // namespace

MapSystem::MapSystem()
    : Scene() {
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

        for (const auto& player : this->m_Players) {
            this->ApplyCameraRecursive(player);
        }
        for (const auto& room : this->m_Rooms) {
            this->ApplyCameraRecursive(room);
        }
        for (const auto& gangway : this->m_Gangways) {
            this->ApplyCameraRecursive(gangway);
        }
        for (const auto& mob : this->m_Mobs) {
            this->ApplyCameraRecursive(mob);
        }
        for (const auto& bullet : this->m_Bullets) {
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
    if (this->m_Players.empty()) {
        return false;
    }

    std::shared_ptr<BaseRoom> room = this->m_CurrentRoom;
    if (room == nullptr) {
        room = this->FindRoomByPlayerPosition(this->m_Players.front()->GetAbsoluteTranslation());
    }

    return room != nullptr;
}

glm::vec2 MapSystem::GetCameraCoor() const {
    if (this->m_AttachCamera == nullptr) {
        return {0.0F, 0.0F};
    }

    return this->m_AttachCamera->GetCooridinate();
}

void MapSystem::AddRoom(const std::shared_ptr<BaseRoom> &room) {
    if (room == nullptr) {
        return;
    }

    this->m_Rooms.push_back(room);
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

    this->m_Gangways.push_back(gangway);
}

void MapSystem::AddGangways(const std::vector<std::shared_ptr<Gangway>> &gangways) {
    for (const auto &gangway : gangways) {
        this->AddGangway(gangway);
    }
}

Collision::MovementResult MapSystem::ResolvePlayerMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) {
    this->UpdateCurrentRoom(body.GetCollisionOrigin());
    this->PrepareDoorPassage(body.GetCollisionOrigin() + intendedDelta);

    if (this->m_CurrentRoom == nullptr &&
        this->m_DoorPassage.state == DoorPassageState::Idle) {
        return {
            intendedDelta,
            false,
            false,
            nullptr,
            nullptr
        };
    }

    const Collision::MovementResult result = this->ResolveMapMovement(body, intendedDelta);
    this->UpdateCurrentRoom(body.GetCollisionOrigin() + result.resolvedDelta);
    return result;
}

Collision::MovementResult MapSystem::ResolveMapMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) const {
    Collision::CollisionQueryOptions options;
    options.ignoreOwner = &body;
    options.blockerMask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop);

    const std::vector<Collision::CollisionPrimitive> blockingPrimitives =
        this->CollectCurrentRoomCollisionPrimitives(&body);
    return this->m_CollisionSystem.ResolveMovement(
        body,
        intendedDelta,
        blockingPrimitives,
        options
    );
}

Collision::MovementResult MapSystem::ResolveProjectileMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) const {
    Collision::CollisionQueryOptions options;
    options.ignoreOwner = &body;
    options.blockerMask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop);

    const std::vector<Collision::CollisionPrimitive> blockingPrimitives =
        this->CollectCurrentRoomCollisionPrimitives(&body);
    return this->m_CollisionSystem.ResolveMovement(
        body,
        intendedDelta,
        blockingPrimitives,
        options
    );
}

Collision::MovementResult MapSystem::PredictMovement(
    const ICollidable &body,
    const glm::vec2 &intendedDelta
) const {
    Collision::CollisionQueryOptions options;
    options.ignoreOwner = &body;
    options.blockerMask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop);

    const std::vector<Collision::CollisionPrimitive> blockingPrimitives =
        this->CollectCurrentRoomCollisionPrimitives(&body);
    return this->m_CollisionSystem.PredictMovement(
        body,
        intendedDelta,
        blockingPrimitives,
        options
    );
}

bool MapSystem::CanOccupy(
    const ICollidable &body,
    const glm::vec2 &targetOrigin
) const {
    Collision::CollisionQueryOptions options;
    options.ignoreOwner = &body;
    options.blockerMask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop);

    const std::vector<Collision::CollisionPrimitive> blockingPrimitives =
        this->CollectCurrentRoomCollisionPrimitives(&body);
    return this->m_CollisionSystem.CanOccupy(
        body,
        targetOrigin,
        blockingPrimitives,
        options
    );
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

    std::vector<Collision::CollisionPrimitive> colliders =
        ToWorldPrimitives(room->GetStaticColliders());
    const std::vector<Collision::CollisionPrimitive> dynamicColliders =
        ToWorldPrimitives(room->GetDynamicColliders(ignoreOverlapBox));

    colliders.insert(
        colliders.end(),
        dynamicColliders.begin(),
        dynamicColliders.end()
    );

    return colliders;
}

std::vector<Collision::CollisionPrimitive> MapSystem::CollectCurrentRoomCollisionPrimitives(
    const ICollidable *ignoreBody
) const {
    if (this->m_CurrentRoom == nullptr &&
        this->m_DoorPassage.state == DoorPassageState::Idle) {
        return {};
    }

    std::vector<Collision::CollisionPrimitive> colliders;
    std::unordered_set<const Gangway *> collectedGangways;

    const std::vector<Collision::CollisionPrimitive> currentRoomColliders =
        this->CollectRoomCollisionPrimitives(this->m_CurrentRoom, ignoreBody);
    colliders.insert(
        colliders.end(),
        currentRoomColliders.begin(),
        currentRoomColliders.end()
    );

    if (this->m_CurrentRoom != nullptr) {
        for (const auto &gangway : this->m_Gangways) {
            if (gangway == nullptr ||
                !gangway->ConnectsRoom(this->m_CurrentRoom) ||
                !collectedGangways.insert(gangway.get()).second) {
                continue;
            }

            const auto &gangwayColliders = gangway->GetStaticColliders();
            const std::vector<Collision::CollisionPrimitive> gangwayPrimitives =
                ToWorldPrimitives(gangwayColliders);
            colliders.insert(
                colliders.end(),
                gangwayPrimitives.begin(),
                gangwayPrimitives.end()
            );
        }
    }

    if (this->m_DoorPassage.state == DoorPassageState::Traversing &&
        this->m_DoorPassage.targetRoom != nullptr &&
        this->m_DoorPassage.targetRoom != this->m_CurrentRoom) {
        const std::vector<Collision::CollisionPrimitive> targetRoomColliders =
            this->CollectRoomCollisionPrimitives(this->m_DoorPassage.targetRoom, ignoreBody);
        colliders.insert(
            colliders.end(),
            targetRoomColliders.begin(),
            targetRoomColliders.end()
        );

        for (const auto &gangway : this->m_Gangways) {
            if (gangway == nullptr ||
                !gangway->ConnectsRoom(this->m_DoorPassage.targetRoom) ||
                !collectedGangways.insert(gangway.get()).second) {
                continue;
            }

            const auto &gangwayColliders = gangway->GetStaticColliders();
            const std::vector<Collision::CollisionPrimitive> gangwayPrimitives =
                ToWorldPrimitives(gangwayColliders);
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

    bodies.reserve(this->m_Players.size() + this->m_Mobs.size() + this->m_Bullets.size());

    for (const auto &player : this->m_Players) {
        if (player != nullptr) {
            bodies.push_back(static_cast<ICollidable *>(player.get()));
        }
    }

    for (const auto &mob : this->m_Mobs) {
        if (mob != nullptr) {
            bodies.push_back(static_cast<ICollidable *>(mob.get()));
        }
    }

    for (const auto &bullet : this->m_Bullets) {
        if (bullet != nullptr) {
            bodies.push_back(static_cast<ICollidable *>(bullet.get()));
        }
    }

    return bodies;
}

bool MapSystem::HasRoomPassageBetween(
    const std::shared_ptr<BaseRoom> &sourceRoom,
    const std::shared_ptr<BaseRoom> &targetRoom,
    DoorSide &targetEntrySide
) const {
    if (sourceRoom == nullptr || targetRoom == nullptr) {
        return false;
    }

    const glm::vec2 delta =
        targetRoom->GetAbsoluteTranslation() - sourceRoom->GetAbsoluteTranslation();
    DoorSide sourceExitSide = DoorSide::Bottom;

    if (std::abs(delta.x) > std::abs(delta.y)) {
        if (std::abs(delta.y) > kRoomAlignmentTolerance ||
            std::abs(delta.x) <= kRoomAlignmentTolerance) {
            return false;
        }

        if (delta.x > 0.0F) {
            sourceExitSide = DoorSide::Right;
            targetEntrySide = DoorSide::Left;
        } else {
            sourceExitSide = DoorSide::Left;
            targetEntrySide = DoorSide::Right;
        }
    } else {
        if (std::abs(delta.x) > kRoomAlignmentTolerance ||
            std::abs(delta.y) <= kRoomAlignmentTolerance) {
            return false;
        }

        if (delta.y > 0.0F) {
            sourceExitSide = DoorSide::Top;
            targetEntrySide = DoorSide::Bottom;
        } else {
            sourceExitSide = DoorSide::Bottom;
            targetEntrySide = DoorSide::Top;
        }
    }

    return sourceRoom->HasPassageOnSide(sourceExitSide) &&
           targetRoom->HasPassageOnSide(targetEntrySide);
}

bool MapSystem::TryStartDoorPassage(const std::shared_ptr<BaseRoom> &targetRoom) {
    if (this->m_CurrentRoom == nullptr ||
        targetRoom == nullptr ||
        targetRoom == this->m_CurrentRoom) {
        return false;
    }

    DoorSide targetEntrySide = DoorSide::Bottom;
    if (!this->HasRoomPassageBetween(this->m_CurrentRoom, targetRoom, targetEntrySide)) {
        return false;
    }

    this->m_DoorPassage.state = DoorPassageState::Traversing;
    this->m_DoorPassage.sourceRoom = this->m_CurrentRoom;
    this->m_DoorPassage.targetRoom = targetRoom;
    this->m_DoorPassage.targetEntrySide = targetEntrySide;
    return true;
}

bool MapSystem::HasCommittedDoorPassage(const glm::vec2 &playerPos) const {
    if (this->m_DoorPassage.state != DoorPassageState::Traversing ||
        this->m_DoorPassage.targetRoom == nullptr ||
        !this->m_DoorPassage.targetRoom->IsPlayerInside(playerPos)) {
        return false;
    }

    const glm::vec2 roomCenter = this->m_DoorPassage.targetRoom->GetAbsoluteTranslation();
    const glm::vec2 roomHalfSize = this->m_DoorPassage.targetRoom->GetRoomSize() / 2.0F;

    switch (this->m_DoorPassage.targetEntrySide) {
    case DoorSide::Top:
        return playerPos.y <= roomCenter.y + roomHalfSize.y - kDoorPassageCommitDepth;

    case DoorSide::Right:
        return playerPos.x <= roomCenter.x + roomHalfSize.x - kDoorPassageCommitDepth;

    case DoorSide::Bottom:
        return playerPos.y >= roomCenter.y - roomHalfSize.y + kDoorPassageCommitDepth;

    case DoorSide::Left:
        return playerPos.x >= roomCenter.x - roomHalfSize.x + kDoorPassageCommitDepth;
    }

    return false;
}

void MapSystem::CommitDoorPassage() {
    if (this->m_DoorPassage.state != DoorPassageState::Traversing ||
        this->m_DoorPassage.targetRoom == nullptr) {
        return;
    }

    if (this->m_CurrentRoom != nullptr) {
        this->m_CurrentRoom->OnPlayerLeave();
    }

    this->m_CurrentRoom = this->m_DoorPassage.targetRoom;
    this->m_CurrentRoom->OnPlayerEnter();
    this->CancelDoorPassage();
}

void MapSystem::CancelDoorPassage() {
    this->m_DoorPassage = DoorPassageContext{};
}

void MapSystem::PrepareDoorPassage(const glm::vec2 &playerPos) {
    if (this->m_DoorPassage.state == DoorPassageState::Traversing ||
        this->m_CurrentRoom == nullptr) {
        return;
    }

    const std::shared_ptr<BaseRoom> nextRoom = this->FindRoomByPlayerPosition(playerPos);
    if (nextRoom == nullptr || nextRoom == this->m_CurrentRoom) {
        return;
    }

    this->TryStartDoorPassage(nextRoom);
}

std::shared_ptr<BaseRoom> MapSystem::FindRoomByPlayerPosition(const glm::vec2 &playerPos) const {
    for (const auto &room : this->m_Rooms) {
        if (room != nullptr && room->IsPlayerInside(playerPos)) {
            return room;
        }
    }

    return nullptr;
}

void MapSystem::UpdateCurrentRoom(const glm::vec2 &playerPos) {
    const std::shared_ptr<BaseRoom> nextRoom = this->FindRoomByPlayerPosition(playerPos);

    if (this->m_DoorPassage.state == DoorPassageState::Traversing) {
        if (this->m_DoorPassage.sourceRoom != nullptr &&
            this->m_DoorPassage.sourceRoom->IsPlayerInside(playerPos)) {
            this->CancelDoorPassage();
            return;
        }

        if (this->HasCommittedDoorPassage(playerPos)) {
            this->CommitDoorPassage();
            return;
        }

        if (nextRoom == nullptr ||
            nextRoom == this->m_DoorPassage.targetRoom) {
            return;
        }

        this->CancelDoorPassage();
    }

    if (nextRoom == this->m_CurrentRoom || nextRoom == nullptr) {
        return;
    }

    if (this->m_CurrentRoom != nullptr &&
        this->TryStartDoorPassage(nextRoom)) {
        if (this->HasCommittedDoorPassage(playerPos)) {
            this->CommitDoorPassage();
        }
        return;
    }

    if (this->m_CurrentRoom != nullptr) {
        this->m_CurrentRoom->OnPlayerLeave();
    }

    this->m_CurrentRoom = nextRoom;

    if (this->m_CurrentRoom != nullptr) {
        this->m_CurrentRoom->OnPlayerEnter();
    }
}

const std::vector<std::shared_ptr<Bullet>>& MapSystem::GetBullets() const {
    return this->m_Bullets;
}

void MapSystem::AddBullet(std::shared_ptr<Bullet> bullet) {
    if (bullet != nullptr) {
        bullet->SetCollisionResolver(
            [this](const ICollidable &body, const glm::vec2 &intendedDelta) {
                return this->ResolveProjectileMovement(body, intendedDelta);
            }
        );
    }

    this->AddEntity(bullet, this->m_Bullets, "bullet");
}

void MapSystem::RemoveBullet(std::shared_ptr<Bullet> bullet) {
    this->RemoveEntity(bullet, this->m_Bullets, "bullet");
}

void MapSystem::AddMob(std::shared_ptr<Character> mob) {
    if (mob != nullptr) {
        mob->SetCollisionResolver(
            [this](const ICollidable &body, const glm::vec2 &intendedDelta) {
                return this->ResolveMapMovement(body, intendedDelta);
            }
        );
    }

    this->AddEntity(mob, this->m_Mobs, "mob");
}

void MapSystem::RemoveMob(std::shared_ptr<Character> mob) {
    this->RemoveEntity(mob, this->m_Mobs, "mob");
}

const std::vector<std::shared_ptr<Character>>& MapSystem::GetMob() const {
    return this->m_Mobs;
}

void MapSystem::PruneDestroyedBullets() {
    std::vector<std::shared_ptr<Bullet>> destroyedBullets;

    for (const auto &bullet : this->m_Bullets) {
        if (bullet != nullptr && bullet->IsDestroyRequested()) {
            destroyedBullets.push_back(bullet);
        }
    }

    for (const auto &bullet : destroyedBullets) {
        this->RemoveBullet(bullet);
    }
}
