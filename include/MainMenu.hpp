#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <imgui.h>
#include <memory>

#include "Component/UI/SettingsUI.hpp"
#include "Util/GameObject.hpp"

#include "Scene.hpp"
#include "Component/Button/TextButton.hpp"

class MainMenu : public Scene {
public:
    MainMenu();
    ~MainMenu() override;

    std::shared_ptr<Scene> GetRedirection() override;

    void Update() override;

private:
    void StartNewGame();
    void LeaveGame();

    void LaunchSettingsUI(bool launch);

    std::shared_ptr<TextButton> m_Button_NewGame;
    std::shared_ptr<TextButton> m_Button_LoadGame;
    std::shared_ptr<TextButton> m_Button_Credit;
    std::shared_ptr<TextButton> m_Button_Leave;

    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_Game_Title;

    std::shared_ptr<Scene> m_Redirect_Scene;

    std::shared_ptr<BaseUI> m_SettingsUI = nullptr;

    std::shared_ptr<Util::BGM> m_BGM;
};

#endif
