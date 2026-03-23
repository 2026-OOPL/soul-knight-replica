#include "Component/Map/MapPiece.hpp"

#include <glm/fwd.hpp>

#include "Util/Transform.hpp"

MapPiece::MapPiece(
    glm::vec2 cooridinate,
    const std::shared_ptr<Core::Drawable> &drawable,
    bool isWall
)
    : m_Cooridinate(cooridinate),
      m_IsWall(isWall) {
    this->SetDrawable(drawable);
    this->m_Image = std::dynamic_pointer_cast<Util::Image>(drawable);

    if (drawable != nullptr) {
        this->m_ColliderSize = this->GetScaledSize();
    }
}

MapPiece::MapPiece(glm::vec2 cooridinate, std::string resource, bool isWall)
    : MapPiece(
          cooridinate,
          std::make_shared<Util::Image>(resource),
          isWall
      ) {
}

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
    const glm::vec2 scaledCooridinate = this->GetPosition();

    this->m_Transform.translation = scaledCooridinate - cooridinate;
}
