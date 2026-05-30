#include <memory>

#include <glm/vec2.hpp>
#include "Util/GameObject.hpp"

#include "Component/UI/SettingsUI.hpp"
#include "Component/Button/ImageButton.hpp"

SettingsUI::SettingsUI(float zIndex)
: Util::GameObject(nullptr, zIndex)
{
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/Pause/layer_dim.png"
        ),
        m_ZIndex
    );

    this->AddChild(m_Background);

    m_SettingsMenu = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/Settings/settings_ui_menu.png",
            false
        ),
        m_ZIndex+1
    );
    m_SettingsMenu->m_Transform.scale = {.5F, .5F};

    this->AddChild(m_SettingsMenu);

    m_CloseButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
          nullptr,
          nullptr,
          [this] () {this->m_ExitSignal = true;}  
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR"/UI/Settings/settings_ui_button.png",
            RESOURCE_DIR"/UI/Settings/settings_ui_button.png",
            RESOURCE_DIR"/UI/Settings/settings_ui_button.png"
        ),
        std::make_shared<ButtonHitbox>(
            glm::vec2(57, 57),
            glm::vec2(365, 200)
        )
    );
    m_CloseButton->m_Transform.scale = {.5F, .5F};
    m_CloseButton->SetZIndex(m_ZIndex+2);

    this->AddChild(m_CloseButton);
}

void SettingsUI::Update() {

    m_CloseButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
          nullptr,
          nullptr,
          [this] () {this->m_ExitSignal = true;}  
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR"/UI/Settings/settings_ui_button.png",
            RESOURCE_DIR"/UI/Settings/settings_ui_button.png",
            RESOURCE_DIR"/UI/Settings/settings_ui_button.png"
        ),
        std::make_shared<ButtonHitbox>(
            glm::vec2(57, 57),
            glm::vec2(365, 200)
        )
    );
    m_CloseButton->m_Transform.scale = {.5F, .5F};
    m_CloseButton->SetZIndex(m_ZIndex+2);

    this->AddChild(m_CloseButton);

    m_MasterVolumeKnob = std::make_shared<ImageButton>(
        nullptr,
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR"/UI/Settings/settings_ui_slider_button.png",
            RESOURCE_DIR"/UI/Settings/settings_ui_slider_button.png",
            RESOURCE_DIR"/UI/Settings/settings_ui_slider_button.png"
        ),
        nullptr
    );

    m_MasterVolumeKnob->m_Transform.scale = {.5F, .5F};
    m_MasterVolumeKnob->m_Transform.translation = glm::vec2(-150, 25);
    m_MasterVolumeKnob->SetZIndex(m_ZIndex+4);

    this->AddChild(m_MasterVolumeKnob);

    m_MasterVolumeProgress = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/Settings/settings_ui_slider_active.png",
            false
        ),
        m_ZIndex+3
    );

    m_MasterVolumeProgress->m_Transform.scale = {.5F, .5F};
    m_MasterVolumeProgress->m_Transform.translation = glm::vec2(-150, 24.5);
    this->AddChild(m_MasterVolumeProgress);
}

bool SettingsUI::GetExitSignal() {
    return m_ExitSignal;
}