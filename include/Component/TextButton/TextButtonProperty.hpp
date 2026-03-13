#include <Util/Color.hpp>
#include <functional>

const Util::Color DefaultNormalColor = Util::Color(238, 238, 238);
const Util::Color DefaultHoverColor = Util::Color(203, 203, 203);
const Util::Color DefaultPressedColor = Util::Color(124, 124, 124);
const std::string DefaultFont = RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf";
const int         DefaultFontSize = 36;

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