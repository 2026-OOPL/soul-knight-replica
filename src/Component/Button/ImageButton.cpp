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
            image->SetImage(this->theme->pressed);
            break;
        case ButtonState::HOVER:
            image->SetImage(this->theme->hover);
            break;
        default:
            image->SetImage(this->theme->normal);
            break;
    }
}

glm::vec2 ImageButton::GetButonHitboxSize() {
    if (m_Size == glm::vec2(-114514, -228922)) {
        throw std::runtime_error("GetButonHitboxSize not override and static size was not set");
    }

    return this->m_Size;
}

glm::vec2 ImageButton::GetButonHitboxTranslation() {
    if (m_Translation == glm::vec2(-114514, -228922)) {
        throw std::runtime_error("GetButonHitboxTranslation not override and static translation was not set");
    }

    return this->m_Translation;
}