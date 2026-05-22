#ifndef IMAGE_BUTTON_HPP
#define IMAGE_BUTTON_HPP

#include <memory>
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

class ImageButton : public Button {
public:
    ImageButton(
        std::shared_ptr<ButtonAction> action,
        std::shared_ptr<ImageButtonTheme> theme,
        glm::vec2 size,
        glm::vec2 translation
    ) : Button(action) {
        this->theme = theme != nullptr ? theme : this->theme;

        this->m_Image = std::make_shared<Util::Image>(
            theme->normal
        );

        this->SetDrawable(m_Image);

        this->m_Size = size;
        this->m_Translation = translation;
    }

    ImageButton(
        std::shared_ptr<ImageButtonTheme> theme
    ) : ImageButton(
        nullptr, theme, {0,0}, {0,0}
    ) {};

    virtual glm::vec2 GetButonHitboxSize() override;
    virtual glm::vec2 GetButonHitboxTranslation() override;

    void Update() override;

private:
    glm::vec2 m_Size = {-114514, -228922};
    glm::vec2 m_Translation = {-114514, -228922};

    std::shared_ptr<Util::Image> m_Image;
    std::shared_ptr<ImageButtonTheme> theme;
};

#endif
