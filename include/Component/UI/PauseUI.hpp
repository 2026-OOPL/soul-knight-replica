#ifndef PAUSE_UI_HPP
#define PAUSE_UI_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Button/ImageButton.hpp"
#include "Component/UI/BaseUI.hpp"
#include "Component/UI/SettingsUI.hpp"
#include "Util/GameObject.hpp"

class PauseUI : public BaseUI {
public:
    PauseUI(
        std::function<void()> onHomeButtonClick
    );

    ~PauseUI() override = default;

    void Update() override;

    void ToggleSettings();

private:
    bool m_SettingsLaunched = false;

    std::shared_ptr<SettingsUI> m_SettingsMenu;

    std::shared_ptr<Util::GameObject> m_PauseMenu;
    std::shared_ptr<ImageButton> m_HomeButton;
    std::shared_ptr<ImageButton> m_ContinueButton;
    std::shared_ptr<ImageButton> m_SettingsButton;

    bool isInsideHomeButton();
    bool isInsideContinueButton();
    bool isInsideSettingsButton();

    std::function<void()> onHomeButtonClick;
};

#endif 