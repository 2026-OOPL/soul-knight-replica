#include "Component/Map/MapSystem.hpp"

namespace {

constexpr float kDoorPassageCommitDepth = 32.0F;
constexpr float kRoomAlignmentTolerance = 1.0F;

} // namespace

MapSystem::MapSystem()
    : Scene() {
    this->m_CollisionSystem.SetBlockingBoxProvider(
        [this]() {
            return this->CollectCurrentRoomColliders();
        }
    );
}

bool MapSystem::IsPlayerInsideRoom() const {
    if (this->m_Players.empty()) {
        return false;
    }

    std::shared_ptr<BaseRoom> room = this->m_CurrentRoom;
    if (room == nullptr) {
        room = this->FindRoomByPlayerPosition(this->m_Players.front()->GetAbsolutePosition());
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

Collision::MovementResult MapSystem::ResolvePlayerMovement(
    const Collision::AxisAlignedBox &currentBox,
    const glm::vec2 &intendedDelta
) {
    this->UpdateCurrentRoom(currentBox.center);
    this->PrepareDoorPassage(currentBox.center + intendedDelta);

    if (this->m_CurrentRoom == nullptr &&
        this->m_DoorPassage.state == DoorPassageState::Idle) {
        return {
            intendedDelta,
            false,
            false
        };
    }

    const Collision::MovementResult result =
        this->m_CollisionSystem.ResolveMovement(currentBox, intendedDelta);
    this->UpdateCurrentRoom(currentBox.center + result.resolvedDelta);
    return result;
}

std::vector<Collision::AxisAlignedBox> MapSystem::CollectRoomColliders(
    const std::shared_ptr<BaseRoom> &room,
    const Collision::AxisAlignedBox *playerBox
) const {
    if (room == nullptr) {
        return {};
    }

    std::vector<Collision::AxisAlignedBox> colliders = room->GetStaticColliders();
    const std::vector<Collision::AxisAlignedBox> dynamicColliders =
        room->GetDynamicColliders(playerBox);

    colliders.insert(
        colliders.end(),
        dynamicColliders.begin(),
        dynamicColliders.end()
    );

    return colliders;
}

std::vector<Collision::AxisAlignedBox> MapSystem::CollectCurrentRoomColliders() const {
    if (this->m_CurrentRoom == nullptr &&
        this->m_DoorPassage.state == DoorPassageState::Idle) {
        return {};
    }

    const Collision::AxisAlignedBox *playerBox = nullptr;
    Collision::AxisAlignedBox currentPlayerBox;
    std::vector<Collision::AxisAlignedBox> colliders;

    if (!this->m_Players.empty() && this->m_Players.front() != nullptr) {
        currentPlayerBox = this->m_Players.front()->GetCollisionBox();
        playerBox = &currentPlayerBox;
    }

    const std::vector<Collision::AxisAlignedBox> currentRoomColliders =
        this->CollectRoomColliders(this->m_CurrentRoom, playerBox);
    colliders.insert(
        colliders.end(),
        currentRoomColliders.begin(),
        currentRoomColliders.end()
    );

    if (this->m_DoorPassage.state == DoorPassageState::Traversing &&
        this->m_DoorPassage.targetRoom != nullptr &&
        this->m_DoorPassage.targetRoom != this->m_CurrentRoom) {
        const std::vector<Collision::AxisAlignedBox> targetRoomColliders =
            this->CollectRoomColliders(this->m_DoorPassage.targetRoom, playerBox);
        colliders.insert(
            colliders.end(),
            targetRoomColliders.begin(),
            targetRoomColliders.end()
        );
    }

    return colliders;
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
        targetRoom->GetAbsoluteCooridinate() - sourceRoom->GetAbsoluteCooridinate();
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

    const glm::vec2 roomCenter = this->m_DoorPassage.targetRoom->GetAbsoluteCooridinate();
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
