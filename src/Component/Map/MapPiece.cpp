#include "Component/Map/MapPiece.hpp"
#include "Util/Transform.hpp"
#include <glm/fwd.hpp>


glm::vec2 MapPiece::GetObjectSize() {
    return this->GetScaledSize();
}

glm::vec2 MapPiece::GetCooridinate() {
    return this->m_Cooridinate;
}

Util::Transform MapPiece::GetTransform() {
    return this->m_Transform;
}

void MapPiece::SetTransformByCooridinate(glm::vec2 cooridinate) {
    glm::vec2 scaledCooridinate = GetPosition();

    this->m_Transform.translation = scaledCooridinate - cooridinate;
}