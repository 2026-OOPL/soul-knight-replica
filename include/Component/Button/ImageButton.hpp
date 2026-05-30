#ifndef IMAGE_BUTTON_HPP
#define IMAGE_BUTTON_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include "Component/Button/Button.hpp"
#include "Util/Image.hpp"
#include <Util/Color.hpp>
#include <Util/GameObject.hpp>
#include <Util/Text.hpp>

typedef struct ImageButtonTheme {
    ImageButtonTheme(
        std::string normal,
        std::string hover,
        std::string pressed
    ) : normal(normal), hover(hover), pressed(pressed) {}

    std::string normal;
    std::string hover;
    std::string pressed;

} ImageButtonTheme_t;

typedef struct ButtonHitbox {
    glm::vec2 size;
    glm::vec2 translation;

    ButtonHitbox(
        glm::vec2 size,
        glm::vec2 translation
    ) : size(size), translation(translation) {}
} ButtonHitbox_t;

class ImageButton : public Button {
public:
    ImageButton(
        std::shared_ptr<ButtonAction> action,
        std::shared_ptr<ImageButtonTheme> theme,
        std::shared_ptr<ButtonHitbox> hitbox
    ) : Button(action) {
        if (theme == nullptr) {
            throw std::runtime_error("Image button should have a theme.");
        }
        
        this->m_Theme = theme;
        this->m_HitBox = hitbox;

        this->m_Image = std::make_shared<Util::Image>(
            theme->normal,
            false
        );

        this->SetDrawable(m_Image);
    }

    ImageButton(
        std::shared_ptr<ImageButtonTheme> theme
    ) : ImageButton(
        nullptr, theme, nullptr
    ) {};

    virtual glm::vec2 GetButonHitboxSize() override;
    virtual glm::vec2 GetButonHitboxTranslation() override;

    void Update() override;

private:
    std::shared_ptr<Util::Image> m_Image;

    std::shared_ptr<ButtonHitbox> m_HitBox;
    std::shared_ptr<ImageButtonTheme> m_Theme;
};

#endif
