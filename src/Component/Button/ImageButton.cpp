#include <glm/ext/vector_float2.hpp>
#include <imgui.h>
#include <memory>
#include <stdexcept>

#include "Component/Button/ImageButton.hpp"

void ImageButton::Update() {
    Button::Update();

    std::shared_ptr<Util::Image> image = std::dynamic_pointer_cast<Util::Image>(this->m_Drawable);

    switch (this->GetButtonState()) {
        case ButtonState::PRESSED:
            image->SetImage(this->m_Theme->pressed);
            break;
        case ButtonState::HOVER:
            image->SetImage(this->m_Theme->hover);
            break;
        default:
            image->SetImage(this->m_Theme->normal);
            break;
    }
}

glm::vec2 ImageButton::GetButonHitboxSize() {
    if (m_HitBox == nullptr) {
        return this->GetScaledSize();
    }

    return this->m_HitBox->size;
}

glm::vec2 ImageButton::GetButonHitboxTranslation() {
    if (m_HitBox == nullptr) {
        return this->GetScaledSize();
    }

    return this->m_HitBox->translation;
}