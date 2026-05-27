#ifndef TEXT_BUTTON_HPP
#define TEXT_BUTTON_HPP

#include <memory>

#include "Component/Button/Button.hpp"
#include <Util/Color.hpp>
#include <Util/GameObject.hpp>
#include <Util/Text.hpp>

namespace {

const Util::Color DefaultNormalColor = Util::Color(238, 238, 238);
const Util::Color DefaultHoverColor = Util::Color(203, 203, 203);
const Util::Color DefaultPressedColor = Util::Color(124, 124, 124);
const std::string DefaultFont = RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf";
const int         DefaultFontSize = 36;

}

typedef struct TextButtonTheme {
    Util::Color normal;
    Util::Color hover;
    Util::Color pressed;

    std::string font;
    int size;

    TextButtonTheme(
    ) : normal(DefaultNormalColor), 
        hover(DefaultHoverColor),
        pressed(DefaultPressedColor), 
        font(DefaultFont),
        size(DefaultFontSize) {}

    TextButtonTheme(
        std::string font,
        int size
    ) : normal(DefaultNormalColor), 
        hover(DefaultHoverColor),
        pressed(DefaultPressedColor), 
        font(font),
        size(size) {}

    TextButtonTheme(
        Util::Color normal,
        Util::Color hover,
        Util::Color pressed
    ) : normal(normal), 
        hover(hover), 
        pressed(pressed), 
        font(DefaultFont), 
        size(DefaultFontSize) {}
} TextButtonTheme_t;

class TextButton : public Button {
public:
    TextButton(
        const std::string &text,
        std::shared_ptr<ButtonAction> action,
        std::shared_ptr<TextButtonTheme> theme
    ) : Button(action) {
        this->theme = theme != nullptr ? theme : this->theme;

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
        std::shared_ptr<ButtonAction> action
    ) : TextButton(
        text, action, nullptr
    ) {};

    glm::vec2 GetButonHitboxSize() override;
    glm::vec2 GetButonHitboxTranslation() override;

    void Update() override;

private:
    std::shared_ptr<Util::Text> m_Text;
    std::shared_ptr<TextButtonTheme> theme = std::make_shared<TextButtonTheme>();
};

#endif
