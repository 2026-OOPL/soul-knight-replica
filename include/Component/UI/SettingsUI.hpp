#ifndef COMPONENT_UI_SETTINGS_UI_HPP
#define COMPONENT_UI_SETTINGS_UI_HPP

#include <memory>

#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Component/Slider/Slider.hpp"
#include "Util/GameObject.hpp"

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
    
    std::shared_ptr<Slider> m_MasterVolumeSilder;
    std::shared_ptr<Slider> m_SFXVolumeSilder;
};

#endif
