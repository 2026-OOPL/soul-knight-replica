#ifndef TEXT_BUTTON_HPP
#define TEXT_BUTTON_HPP

#include <Util/GameObject.hpp>
#include <Util/Text.hpp>

#include <memory>

#include "Component/IStateful.hpp"
#include <Util/Color.hpp>
#include <functional>

namespace {

const Util::Color DefaultNormalColor = Util::Color(238, 238, 238);
const Util::Color DefaultHoverColor = Util::Color(203, 203, 203);
const Util::Color DefaultPressedColor = Util::Color(124, 124, 124);
const std::string DefaultFont = RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf";
const int         DefaultFontSize = 36;

}

typedef struct ButtonTheme {
    Util::Color normal;
    Util::Color hover;
    Util::Color pressed;

    std::string font;
    int size;

    ButtonTheme(
    ) : normal(DefaultNormalColor), 
        hover(DefaultHoverColor),
        pressed(DefaultPressedColor), 
        font(DefaultFont),
        size(DefaultFontSize) {}

    ButtonTheme(
        std::string font,
        int size
    ) : normal(DefaultNormalColor), 
        hover(DefaultHoverColor),
        pressed(DefaultPressedColor), 
        font(font),
        size(size) {}

    ButtonTheme(
        Util::Color normal,
        Util::Color hover,
        Util::Color pressed
    ) : normal(normal), 
        hover(hover), 
        pressed(pressed), 
        font(DefaultFont), 
        size(DefaultFontSize) {}
} ButtonTheme_t;

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
