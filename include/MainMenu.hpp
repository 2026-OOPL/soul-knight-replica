#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <imgui.h>
#include <memory>

#include "Util/GameObject.hpp"

#include "Scene.hpp"
#include "Component/TextButton/TextButton.hpp"

class MainMenu : public Scene {
public:
    MainMenu();
    ~MainMenu() override;

    std::shared_ptr<Scene> GetRedirection() override;

private:
    void StartNewGame();
    void LeaveGame();

    std::shared_ptr<TextButton> m_Button_NewGame;
    std::shared_ptr<TextButton> m_Button_LoadGame;
    std::shared_ptr<TextButton> m_Button_Credit;
    std::shared_ptr<TextButton> m_Button_Leave;

    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_Game_Title;

    std::shared_ptr<Scene> m_Redirect_Scene;
};

#endif
