#include <glm/fwd.hpp>

#include "Util/Transform.hpp"

#include "Component/Map/MapPiece.hpp"

MapPiece::MapPiece(
    glm::vec2 cooridinate,
    const std::shared_ptr<Core::Drawable> &drawable
) {
    this->m_AbsoluteTransform.translation = cooridinate;

    this->SetDrawable(drawable);
    this->m_Image = std::dynamic_pointer_cast<Util::Image>(drawable);

    if (drawable != nullptr) {
        this->m_ColliderSize = this->GetScaledSize();
    }
}

MapPiece::MapPiece(glm::vec2 cooridinate, std::string resource)
    : MapPiece(
          cooridinate,
          std::make_shared<Util::Image>(resource, false)
      ) {
}

Util::Transform MapPiece::GetObjectTransform() const {
    return this->m_Transform;
}
