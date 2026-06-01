#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/UI/PauseUI.hpp"
#include "Component/Button/Button.hpp"
#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Component/UI/SettingsUI.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Transform.hpp"

PauseUI::PauseUI(std::function<void()> onHomeButtonClick)
 : BaseUI(true) {
    const int zIndexBase = this->GetZIndex();

    this->onHomeButtonClick = onHomeButtonClick;
    
    this->m_Transform.scale = {2.0F, 2.0F};

    m_PauseMenu = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/Pause/menu_pause.png",
            false
        ),
        zIndexBase+2
    );

    this->AddChild(m_PauseMenu);

    m_HomeButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this] () {
                if (m_SettingsLaunched) return;
                this->onHomeButtonClick();
            }
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR"/UI/Pause/button_home.png",
            RESOURCE_DIR"/UI/Pause/button_home_hovered.png",
            RESOURCE_DIR"/UI/Pause/button_home_pressed.png"
        ),
        std::make_shared<ButtonHitbox>(
            glm::vec2(79, 33) * this->m_Transform.scale,
            this->m_Transform.translation + glm::vec2(-101.5, -45.5) * this->m_Transform.scale
        )
    );

    m_HomeButton->SetZIndex(zIndexBase+3);
    this->AddChild(m_HomeButton);

    m_ContinueButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this] () {
                if (this->m_SettingsLaunched) return;
                this->m_ExitSignal = true;
            }
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR"/UI/Pause/button_continue.png",
            RESOURCE_DIR"/UI/Pause/button_continue_hovered.png",
            RESOURCE_DIR"/UI/Pause/button_continue_pressed.png"
        ),
        std::make_shared<ButtonHitbox>(
            glm::vec2(124, 33) * this->m_Transform.scale,
            this->m_Transform.translation + glm::vec2(0, -45.5) * this->m_Transform.scale
        )
    );

    m_ContinueButton->SetZIndex(zIndexBase+3);
    this->AddChild(m_ContinueButton);

    m_SettingsButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this] () {
                if (this->m_SettingsLaunched) return;
                this->ToggleSettings();
            }
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR"/UI/Pause/button_settings.png",
            RESOURCE_DIR"/UI/Pause/button_settings_hovered.png",
            RESOURCE_DIR"/UI/Pause/button_settings_pressed.png"
        ),
        std::make_shared<ButtonHitbox>(
            glm::vec2(79, 33) * this->m_Transform.scale,
            this->m_Transform.translation + glm::vec2(101.5, -45.5) * this->m_Transform.scale
        )
    );

    m_SettingsButton->SetZIndex(zIndexBase+3);
    this->AddChild(m_SettingsButton);

    for (int i=0; i< (int) this->GetChildren().size(); i++) {
        this->GetChildren()[i]->m_Transform = this->m_Transform;
    }
}

void PauseUI::Update() {
    // Listen for settings UI exit signal
    if (m_SettingsMenu && m_SettingsMenu->GetExitSignal()) {
        this->ToggleSettings();
    }
    
    // If settings menu is opened, skip the ESC check
    if (!m_SettingsLaunched && Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        this->m_ExitSignal = true;
    }

    BaseUI::Update();
}

void PauseUI::ToggleSettings() {
    if (m_SettingsLaunched) {
        this->RemoveChild(this->m_SettingsMenu);
        this->m_SettingsMenu = nullptr;
        this->m_SettingsLaunched = false;
        return;
    }

    this->m_SettingsMenu = std::make_shared<SettingsUI>();
    this->m_SettingsMenu->SetZIndex(m_ZIndex + 5);
    this->AddChild(this->m_SettingsMenu);

    this->m_SettingsLaunched = true;
}