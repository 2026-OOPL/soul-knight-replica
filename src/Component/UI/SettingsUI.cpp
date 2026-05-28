#include "Component/UI/SettingsUI.hpp"
#include "Component/Button/ImageButton.hpp"
#include "Util/Input.hpp"
#include <glm/ext/vector_float2.hpp>
#include <memory>

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
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        this->m_ExitSignal = true;
    }

    std::vector<std::shared_ptr<GameObject>> children = this->GetChildren();
    for (int i=0; i< (int) children.size(); i++) {
        std::shared_ptr<IStateful> stateful = std::dynamic_pointer_cast<IStateful>(children[i]);
        if (stateful) { stateful->Update(); }
    }
} 

bool SettingsUI::GetExitSignal() {
    return m_ExitSignal;
}