#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <memory>
#include <functional>

#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"

typedef struct ButtonAction {
    std::function<void()> onEnter;
    std::function<void()> onLeave;
    std::function<void()> onClick;

    ButtonAction(
        std::function<void()> onEnter,
        std::function<void()> onLeave,
        std::function<void()> onClick
    ) : onEnter(onEnter), onLeave(onLeave), onClick(onClick) {}
} ButtonAction_t;

enum class ButtonState {
    NORMAL,
    HOVER,
    PRESSED
};

class Button : public Util::GameObject, public IStateful {
public:
    Button() {};

    Button(
        std::shared_ptr<ButtonAction> action
    ) : GameObject(nullptr, 3) {
        this->action = action != nullptr ? action : this->action;
    }

    virtual glm::vec2 GetButonHitboxSize() = 0;
    virtual glm::vec2 GetButonHitboxTranslation() = 0;

    void Update() override;
    ButtonState GetButtonState();

    private:
    void HandlePress();
    void HandleHover();
    bool isMouseInBound();

    bool isEnter = false;
    bool isPressed = false;
    bool m_PressedInside = false;

    std::shared_ptr<ButtonAction> action = nullptr;
};

#endif