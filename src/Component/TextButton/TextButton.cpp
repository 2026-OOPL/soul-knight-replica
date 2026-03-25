#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Transform.hpp"

#include <Component/TextButton/TextButton.hpp>
#include <imgui.h>
#include <memory>

bool TextButton::isMouseInBound() {
    Util::Transform transform = this->GetTransform();
    glm::vec2 size = this->GetScaledSize();

    float left = transform.translation.x - size.x / 2;
    float right = transform.translation.x + size.x / 2;
    float down = transform.translation.y - size.y / 2;
    float up = transform.translation.y + size.y / 2;

    glm::vec2 cursorPosition = Util::Input::GetCursorPosition();

    return !(
        cursorPosition.x < left ||
        cursorPosition.x > right ||
        cursorPosition.y < down ||
        cursorPosition.y > up
    );
}

void TextButton::HandlePress() {
    const bool isInBound = this->isMouseInBound();
    const bool isMouseDown = Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB);
    const bool isMousePressedThisFrame = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    const bool isMouseReleasedThisFrame = Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB);

    if (isMousePressedThisFrame && isInBound) {
        this->m_PressedInside = true;
        this->isPressed = true;
    }

    if (!isMouseDown && !isMouseReleasedThisFrame) {
        this->isPressed = false;
    }

    if (isMouseReleasedThisFrame) {
        const bool shouldClick =
            isInBound && this->m_PressedInside && this->action && this->action->onClick;

        this->isPressed = false;
        this->m_PressedInside = false;

        if (shouldClick) {
            this->action->onClick();
        }
    }
}

void TextButton::HandleHover() {
    bool isInBound = this->isMouseInBound();

    if (!isInBound && this->isEnter) {
        this->isEnter = false;

        if (this->action && this->action->onLeave) {
            this->action->onLeave();
        }
        return;
    }

    if (isInBound && !this->isEnter) {
        this->isEnter = true;

        if (this->action && this->action->onEnter) {
            this->action->onEnter();
        }

        return;
    }
}

void TextButton::Update() {
    this->HandlePress();
    this->HandleHover();

    if (this->isPressed) {
        std::dynamic_pointer_cast<Util::Text>(this->m_Drawable)->SetColor(this->theme->pressed);
    } else if (this->isEnter) {
        std::dynamic_pointer_cast<Util::Text>(this->m_Drawable)->SetColor(this->theme->hover);
    } else {
        std::dynamic_pointer_cast<Util::Text>(this->m_Drawable)->SetColor(this->theme->normal);
    }
}
