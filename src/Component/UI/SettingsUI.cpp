#include "Component/UI/SettingsUI.hpp"

SettingsUI::SettingsUI() {
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/Pause/layer_dim.png"
        ),
        20
    );

    this->AddChild(m_Background);

    m_SettingsMenu = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/Settings/menu_settings.png",
            false
        ),
        21
    );
    m_SettingsMenu->m_Transform.scale = {.35F, .35F};

    this->AddChild(m_SettingsMenu);
}

void SettingsUI::Update() {
    return; 
} 

bool SettingsUI::GetExitSignal() {
    return m_ExitSignal;
}