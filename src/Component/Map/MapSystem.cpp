#include "Component/Map/MapSystem.hpp"

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

    if (this->m_CurrentRoom == nullptr) {
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

std::vector<Collision::AxisAlignedBox> MapSystem::CollectCurrentRoomColliders() const {
    if (this->m_CurrentRoom == nullptr) {
        return {};
    }

    std::vector<Collision::AxisAlignedBox> colliders =
        this->m_CurrentRoom->GetStaticColliders();
    const std::vector<Collision::AxisAlignedBox> dynamicColliders =
        this->m_CurrentRoom->GetDynamicColliders();

    colliders.insert(
        colliders.end(),
        dynamicColliders.begin(),
        dynamicColliders.end()
    );

    return colliders;
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
    if (nextRoom == this->m_CurrentRoom) {
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
