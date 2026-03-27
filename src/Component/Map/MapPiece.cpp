#include "Component/Map/MapPiece.hpp"

#include <glm/fwd.hpp>
#include <memory>

#include "Core/Drawable.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Transform.hpp"

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
          std::make_shared<Util::Image>(resource)
      ) {
}

glm::vec2 MapPiece::GetAbsoluteScale() {
    return this->m_Drawable->GetSize() * this->m_AbsoluteTransform.scale;
}

Util::Transform MapPiece::GetAbsoluteTransform() {
    return this->m_AbsoluteTransform;
}

Util::Transform MapPiece::GetObjectTransform() {
    return this->m_Transform;
}
