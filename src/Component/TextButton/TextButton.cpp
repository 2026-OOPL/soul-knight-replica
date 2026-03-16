#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Transform.hpp"

#include <Component/TextButton/TextButton.hpp>
#include <imgui.h>
#include <memory>

bool TextButton::isMouseInBound() {
    Util::Transform transform = GetTransform();
    glm::vec2 size = GetScaledSize();
    
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
    bool isInBound = isMouseInBound();

    if (isInBound) {
        isPressed = Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB);

        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB) && action && action->onClick) {
            action->onClick();
        }
    } else {
        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
            isPressed = false;
        }
    }

}

void TextButton::HandleHover() {
    bool isInBound = isMouseInBound();

    if (!isInBound && isEnter) {
        isEnter = false;
        
        if (action && action->onLeave) {
            action->onLeave();
        }
        return;
    }

    if (isInBound && !isEnter) {
        isEnter = true;
        
        if (action && action->onEnter) {
            action->onEnter();
        }

        return;
    }
}

void TextButton::Update() {
    HandlePress();
    HandleHover();

    if (isPressed) {
        std::dynamic_pointer_cast<Util::Text>(m_Drawable) 
        -> SetColor(theme->pressed);
    } else if (isEnter) {
        std::dynamic_pointer_cast<Util::Text>(m_Drawable) 
        -> SetColor(theme->hover);
    } else {
        std::dynamic_pointer_cast<Util::Text>(m_Drawable) 
        -> SetColor(theme->normal);
    }
}
