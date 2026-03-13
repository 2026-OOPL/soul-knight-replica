#include <glm/fwd.hpp>
#include <memory>

#include "Core/Context.hpp"
#include "Scene/MapTest.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Component/Map/MapPiece.hpp"

#include "MainMenu.hpp"
#include "Component/TextButton/TextButton.hpp"

void MainMenu::StartNewGame() {
    LOG_INFO("Game started !");

    std::vector<std::shared_ptr<MapPiece>> test;


    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(0, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(-1280, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(1280, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));
    
    // asdsadsad
    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(0, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(-1280, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(1280, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    // sadasds
    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(0, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(-1280, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    test.push_back(std::make_shared<MapPiece>(
        glm::vec2(1280, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    m_Redirect_Scene = std::make_unique<MapTest>(this->m_Root, glm::vec2(0, 0), test);
}

void MainMenu::LeaveGame() {
    LOG_INFO("Game is about to close !");
    
    auto context = Core::Context::GetInstance();
    context->SetExit(true);
    
}

void MainMenu::Initialize() {
    const glm::vec2 buttonBaseline = {-440, -30};
    
    m_Button_NewGame = std::make_unique<TextButton>(
        "開新遊戲", 2,
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() { this->StartNewGame(); }
        )
    );
    m_Button_NewGame->m_Transform.translation = buttonBaseline + glm::vec2(0, 0);
    m_Root->AddChild(m_Button_NewGame);
    
    m_Button_LoadGame = std::make_unique<TextButton>(
        "載入存檔", 2,
        nullptr
    );
    m_Button_LoadGame->m_Transform.translation = buttonBaseline + glm::vec2(0, -70);
    m_Root->AddChild(m_Button_LoadGame);

    m_Button_Credit = std::make_unique<TextButton>(
        "遊玩設定", 2, nullptr
    );
    m_Button_Credit->m_Transform.translation = buttonBaseline + glm::vec2(0, -140);
    m_Root->AddChild(m_Button_Credit);

    m_Button_Leave = std::make_unique<TextButton>(
        "離開遊戲", 2, 
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() { this->LeaveGame(); }
        )
    );
    m_Button_Leave->m_Transform.translation = buttonBaseline + glm::vec2(0, -210);
    m_Root->AddChild(m_Button_Leave);

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/Image/MainMenu/Background.png"
        ),
        0
    );
    m_Root->AddChild(m_Background);
}

void MainMenu::Dispose() {
    m_Root->RemoveChild(m_Button_NewGame);
    m_Root->RemoveChild(m_Button_LoadGame);
    m_Root->RemoveChild(m_Button_Credit);
    m_Root->RemoveChild(m_Button_Leave);
    m_Root->RemoveChild(m_Background);
}

void MainMenu::Update() {
    m_Button_NewGame->Update();
    m_Button_LoadGame->Update();
    m_Button_Credit->Update();
    m_Button_Leave->Update();
}

std::unique_ptr<Scene> MainMenu::GetRedirection() {
    return std::move(this->m_Redirect_Scene);
} 