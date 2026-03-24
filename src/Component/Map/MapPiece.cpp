#include "Component/Map/MapPiece.hpp"

#include <glm/fwd.hpp>

#include "Util/Transform.hpp"

MapPiece::MapPiece(
    glm::vec2 cooridinate,
    const std::shared_ptr<Core::Drawable> &drawable,
    bool isWall
) {
    this->m_AbsoluteTransform.translation = cooridinate;
    this->m_IsWall = isWall;


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

Util::Transform MapPiece::GetAbsoluteTransform() {
    return this->m_AbsoluteTransform;
}

Util::Transform MapPiece::GetObjectTransform() {
    return this->m_Transform;
}
