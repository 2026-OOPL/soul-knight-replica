#include "Component/Room/Door1x5.hpp"

Door1x5::Door1x5(glm::vec2 cooridinate) : MapPiece(
    cooridinate,
    RESOURCE_DIR "/Map/Wall/Wall_1x5.png"
) {}

glm::vec2 Door1x5::GetColliderSize() const {
    return this->m_ColliderSize * this->m_AbsoluteTransform.scale;
};

void Door1x5::SetColliderSize(const glm::vec2 &colliderSize) {
    this->m_ColliderSize = colliderSize;
}

glm::vec2 Door1x5::GetColliderCooridinate() const {
    return m_AbsoluteTransform.translation;
}