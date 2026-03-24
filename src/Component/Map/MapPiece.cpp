#include "Component/Map/MapPiece.hpp"
#include "Util/Transform.hpp"
#include <glm/fwd.hpp>


glm::vec2 MapPiece::GetObjectSize() {
    return this->GetScaledSize();
}

Util::Transform MapPiece::GetAbsoluteTransform() {
    return this->m_AbsoluteTransform;
}

Util::Transform MapPiece::GetObjectTransform() {
    return this->m_Transform;
}