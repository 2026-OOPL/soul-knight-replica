#include <memory>

#include "Scene/Gameover.hpp"
#include "GameConfig/GameConfig.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "MainMenu.hpp"
#include "Util/GameObject.hpp"

GameoverScene::GameoverScene() {
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR "/Gameover/Gameover_background.png"
        ),
        m_ZIndex + 1
    );

    this->AddChild(m_Background);

    m_ContinueButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this] () {this->m_SceneRedirection = std::make_shared<MainMenu>(); }
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR "/Gameover/Gameover_button_continue.png",
            RESOURCE_DIR "/Gameover/Gameover_button_continue.png",
            RESOURCE_DIR "/Gameover/Gameover_button_continue.png"
        ),
        nullptr
    );

    this->m_ContinueButton->m_Transform.translation = glm::vec2(0.0F, -275.0F);
    this->AddChild(m_ContinueButton);

    this->m_BGM = std::make_shared<Util::BGM>(
        RESOURCE_DIR"/SFX/Nokia Arabic Ringtone.mp3"
    );
    this->m_BGM->LoadMedia(RESOURCE_DIR"/SFX/Nokia Arabic Ringtone.mp3");
    this->m_BGM->SetVolume(GameConfig::GetInstance().m_BGMVolume * 128);
}

void GameoverScene::Update() {
    if (!m_SwitchBGM) {
        this->m_BGM->Play();
        m_SwitchBGM = true;
    }

    Scene::Update();
}

std::shared_ptr<Scene> GameoverScene::GetRedirection() {
    if (this->m_SceneRedirection != nullptr) {
        return this->m_SceneRedirection;
    }

    return Scene::GetRedirection();
}