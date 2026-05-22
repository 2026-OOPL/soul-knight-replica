#ifndef PAUSE_UI_HPP
#define PAUSE_UI_HPP

#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"
#include <glm/vec2.hpp>
#include <memory>

class PauseUI : public Util::GameObject, IStateful {

public:
    PauseUI(
        std::function<void()> onHomeButtonClick
    );

    ~PauseUI() override = default;

    void Update() override;

    bool GetExitSignal();

private:
    bool m_ExitSignal = false;

    std::shared_ptr<Util::GameObject> m_PauseMenu;
    std::shared_ptr<Util::GameObject> m_Background;

    std::shared_ptr<ImageButton> m_HomeButton;
    std::shared_ptr<ImageButton> m_ContinueButton;
    std::shared_ptr<ImageButton> m_SettingsButton;

    bool isInsideHomeButton();
    bool isInsideContinueButton();
    bool isInsideSettingsButton();

    std::function<void()> onHomeButtonClick;
};

#endif 