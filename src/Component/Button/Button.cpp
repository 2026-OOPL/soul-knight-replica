#include "Util/Input.hpp"

#include "Component/Button/Button.hpp"

bool Button::isMouseInBound() {
    glm::vec2 center = this->GetButonHitboxTranslation();
    glm::vec2 size = this->GetButonHitboxSize();

    float left = center.x - size.x / 2;
    float right = center.x + size.x / 2;
    float down = center.y - size.y / 2;
    float up = center.y + size.y / 2;

    Util::PTSDPosition cursorPosition = Util::Input::GetCursorPosition();

    return !(
        cursorPosition.x < left ||
        cursorPosition.x > right ||
        cursorPosition.y < down ||
        cursorPosition.y > up
    );
}

void Button::HandlePress() {
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

void Button::HandleHover() {
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

void Button::Update() {
    this->HandlePress();
    this->HandleHover();
}

ButtonState Button::GetButtonState() {
    if (this->isPressed) {
        return ButtonState::PRESSED;
    }

    if (this->isEnter) {
        return ButtonState::HOVER;
    }

    return ButtonState::NORMAL;
}