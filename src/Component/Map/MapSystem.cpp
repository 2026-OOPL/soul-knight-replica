#include "Component/Map/MapSystem.hpp"

#include <utility>

MapSystem::MapSystem()
    : Scene() {
}

MapSystem::MapSystem(std::vector<std::shared_ptr<MapPiece>> pieces)
    : Scene(),
      m_Pieces(std::move(pieces)) {
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

void MapSystem::AddMapPieces(const std::vector<std::shared_ptr<MapPiece>> &pieces) {
    for (const auto &piece : pieces) {
        this->m_Pieces.push_back(piece);
    }
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
        this->m_CurrentRoom->ResolvePlayerMovement(currentBox, intendedDelta);
    this->UpdateCurrentRoom(currentBox.center + result.resolvedDelta);
    return result;
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
    if (this->m_CurrentRoom != nullptr && this->m_CurrentRoom->IsPlayerInside(playerPos)) {
        return;
    }

    this->m_CurrentRoom = this->FindRoomByPlayerPosition(playerPos);
}
