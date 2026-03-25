#ifndef TEXT_BUTTON_HPP
#define TEXT_BUTTON_HPP

#include <Util/GameObject.hpp>
#include <Util/Text.hpp>

#include <memory>

#include "Component/IStateful.hpp"
#include "TextButtonProperty.hpp"

class TextButton : public Util::GameObject, public IStateful {
public:
    TextButton(
        const std::string &text,
        int zIndex,
        std::shared_ptr<ButtonAction> action,
        std::shared_ptr<ButtonTheme> theme
    )
        : GameObject(nullptr, zIndex) {
        this->theme = theme != nullptr ? theme : this->theme;
        this->action = action != nullptr ? action : this->action;

        this->m_Text = std::make_shared<Util::Text>(
            this->theme->font,
            this->theme->size,
            text,
            this->theme->normal
        );

        this->SetDrawable(this->m_Text);
    }

    TextButton(
        const std::string &text,
        int zIndex,
        std::shared_ptr<ButtonAction> action
    ) : GameObject(nullptr, zIndex) {    
        this->action = action != nullptr ? action : this->action;

        this->m_Text = std::make_shared<Util::Text>(
            this->theme->font,
            this->theme->size,
            text,
            this->theme->normal
        );

        this->SetDrawable(this->m_Text);
    }

    void Update() override;

private:
    void HandlePress();
    void HandleHover();
    bool isMouseInBound();

    bool isEnter = false;
    bool isPressed = false;
    bool m_PressedInside = false;

    std::shared_ptr<Util::Text> m_Text;

    std::shared_ptr<ButtonTheme> theme = std::make_shared<ButtonTheme>();
    std::shared_ptr<ButtonAction> action = nullptr;
};

#endif
