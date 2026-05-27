#include <imgui.h>
#include <memory>

#include "Component/Button/TextButton.hpp"
#include "Util/Text.hpp"
#include "Util/Transform.hpp"

void TextButton::Update() {
    Button::Update();

    std::shared_ptr<Util::Text> text = std::dynamic_pointer_cast<Util::Text>(this->m_Drawable);

    switch (this->GetButtonState()) {
        case ButtonState::PRESSED:
            text->SetColor(this->theme->pressed);
            break;
        case ButtonState::HOVER:
            text->SetColor(this->theme->hover);
            break;
        default:
            text->SetColor(this->theme->normal);
            break;
    }
}

glm::vec2 TextButton::GetButonHitboxTranslation() {
    return this->m_Transform.translation;
}

glm::vec2 TextButton::GetButonHitboxSize() {
    return this->GetScaledSize();
}