#include <glm/vec2.hpp>
#include <memory>

#include "MainMenu.hpp"
#include "Component/TextButton/TextButton.hpp"
#include "Core/Context.hpp"
#include "Scene/MapTest.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

void MainMenu::StartNewGame() {
    this->m_Redirect_Scene = std::make_shared<MapTest>();
}

void MainMenu::LeaveGame() {
    LOG_INFO("Game is about to close !");

    auto context = Core::Context::GetInstance();
    context->SetExit(true);
}

MainMenu::MainMenu() : Scene() {
    const glm::vec2 buttonBaseline = {-440, -30};

    this->m_Button_NewGame = std::make_shared<TextButton>(
        "開新遊戲",
        2,
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() { this->StartNewGame(); }
        )
    );
    this->m_Button_NewGame->m_Transform.translation = buttonBaseline + glm::vec2(0, 0);
    this->AddChild(this->m_Button_NewGame);

    this->m_Button_LoadGame = std::make_shared<TextButton>(
        "載入存檔",
        2,
        nullptr
    );
    this->m_Button_LoadGame->m_Transform.translation = buttonBaseline + glm::vec2(0, -70);
    this->AddChild(this->m_Button_LoadGame);

    this->m_Button_Credit = std::make_shared<TextButton>(
        "遊玩設定",
        2,
        nullptr
    );
    this->m_Button_Credit->m_Transform.translation = buttonBaseline + glm::vec2(0, -140);
    this->AddChild(this->m_Button_Credit);

    this->m_Button_Leave = std::make_shared<TextButton>(
        "離開遊戲",
        2,
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() { this->LeaveGame(); }
        )
    );
    this->m_Button_Leave->m_Transform.translation = buttonBaseline + glm::vec2(0, -210);
    this->AddChild(this->m_Button_Leave);

    this->m_Game_Title = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/UI/MainMenu/GameTitle.png"),
        1
    );
    this->m_Game_Title->m_Transform.translation = {-270, 220};
    this->AddChild(this->m_Game_Title);

    this->m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/UI/MainMenu/Background.png"),
        0
    );
    this->AddChild(this->m_Background);
}

MainMenu::~MainMenu() {
    this->RemoveChild(this->m_Button_NewGame);
    this->RemoveChild(this->m_Button_LoadGame);
    this->RemoveChild(this->m_Button_Credit);
    this->RemoveChild(this->m_Button_Leave);
    this->RemoveChild(this->m_Game_Title);
    this->RemoveChild(this->m_Background);
}

std::shared_ptr<Scene> MainMenu::GetRedirection() {
    return this->m_Redirect_Scene;
}
