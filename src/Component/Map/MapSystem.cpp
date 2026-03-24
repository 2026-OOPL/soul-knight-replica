#include "Component/Map/MapSystem.hpp"
#include "Util/Transform.hpp"

#include <cmath>
#include <utility>

MapSystem::MapSystem()
    : Scene() {
}

MapSystem::MapSystem(std::vector<std::shared_ptr<MapPiece>> pieces)
    : Scene(),
      m_Pieces(std::move(pieces)) {
}

bool MapSystem::IsPlayerInsideRoom() const {
    if (this->m_Pieces.empty() || this->m_Players.empty()) {
        return false;
    }

    const std::shared_ptr<MapPiece> room = this->m_Pieces.front();
    const std::shared_ptr<Player> player = this->m_Players.front();

    if (room == nullptr || player == nullptr) {
        return false;
    }

    
    const Util::Transform roomTransform = room->GetAbsoluteTransform();
    const Util::Transform playerTransform = player->GetAbsoluteTransform();

    const glm::vec2 roomCenter = roomTransform.translation;
    const glm::vec2 roomHalfSize = room->GetObjectSize() / 2.0F;
    const glm::vec2 innerHalfSize =
        roomHalfSize - glm::vec2(this->m_RoomWallThickness * 1.5F);
    const glm::vec2 playerOffset = playerTransform.translation - roomCenter;

    return std::abs(playerOffset.x) <= innerHalfSize.x &&
           std::abs(playerOffset.y) <= innerHalfSize.y;
}

glm::vec2 MapSystem::GetCameraCoor() const {
    if (this->m_AttachCamera == nullptr) {
        return {0.0F, 0.0F};
    }

    return this->m_AttachCamera->GetCooridinate();
}
