#ifndef COMPONENT_UI_SETTINGS_UI_HPP
#define COMPONENT_UI_SETTINGS_UI_HPP

#include <functional>
#include <memory>
#include <vector>

#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Component/Player/Player.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

class SettingsUI : public Util::GameObject, public IStateful {
public:
    SettingsUI(float zIndex);

    ~SettingsUI() override = default;

    void Update() override;

    bool GetExitSignal();

private:
    bool m_ExitSignal = false;

    std::shared_ptr<Util::GameObject> m_SettingsMenu;
    std::shared_ptr<Util::GameObject> m_Background;

    std::shared_ptr<ImageButton> m_CloseButton; 
    
    std::shared_ptr<ImageButton> m_MasterVolumeKnob;
    std::shared_ptr<Util::GameObject> m_MasterVolumeProgress;
};

#endif
