#ifndef PAUSE_UI_HPP
#define PAUSE_UI_HPP

#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Component/UI/SettingsUI.hpp"
#include "Util/GameObject.hpp"
#include <glm/vec2.hpp>
#include <memory>

class PauseUI : public Util::GameObject, IStateful {

public:
    PauseUI(
        std::function<void()> onHomeButtonClick,
        float zIndex
    );

    ~PauseUI() override = default;

    void Update() override;

    bool GetExitSignal();

    void ToggleSettings();

private:
    bool m_ExitSignal = false;
    bool m_SettingsLaunched = false;

    std::shared_ptr<Util::GameObject> m_PauseMenu;
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<SettingsUI> m_SettingsMenu;

    std::shared_ptr<ImageButton> m_HomeButton;
    std::shared_ptr<ImageButton> m_ContinueButton;
    std::shared_ptr<ImageButton> m_SettingsButton;

    bool isInsideHomeButton();
    bool isInsideContinueButton();
    bool isInsideSettingsButton();

    std::function<void()> onHomeButtonClick;
};

#endif 