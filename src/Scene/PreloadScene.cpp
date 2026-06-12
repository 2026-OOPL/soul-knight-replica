#include <memory>

#include "Component/UI/Advertisement.hpp"
#include "Scene/PreloadScene.hpp"
#include "MainMenu.hpp"
#include "Util/BGM.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"

PreloadScene::PreloadScene() {
    this->m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/PreloadScene/background.png",
            true
        ),
        1
    );

    this->AddChild(this->m_Background);

    this->m_Text = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
            20,
            "抵制不良游戏，拒绝盗版游戏。注意自我保护，谨防受骗上当。适度游戏益脑，沉迷游戏伤身。合理安排时间，享受健康生活。",
            Util::Color(66, 66, 66),
            false
        ),
        2
    );

    this->m_Text->m_Transform.translation = {0, -300};
    this->AddChild(this->m_Text);

    m_SceneStartTime = Util::Time::GetElapsedTimeMs();

    this->m_BGM = std::make_shared<Util::BGM>(
        RESOURCE_DIR"/SFX/Startup.mp3"
    );
}

void PreloadScene::Update() {
    
    if (!m_BGMStarted) {
        m_BGM->Play(1);
        m_BGMStarted = true;
    }

    Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (now - m_SceneStartTime > 500) {
        // Advertisement preload
        std::shared_ptr<AdvertisementUI> advertisementUI = std::make_shared<AdvertisementUI>(
            AdvertisementArc::SPRITES,
            AdvertisementArc::SPRITE_NUMBER
        );

        advertisementUI->PreloadMedia();

        this->m_RedirectScene = std::make_shared<MainMenu>();
    }
    
    return;
}

std::shared_ptr<Scene> PreloadScene::GetRedirection() {
    return m_RedirectScene;
}

void PreloadScene::StartPreload() {
    // Advertisement preload
    std::shared_ptr<AdvertisementUI> advertisementUI = std::make_shared<AdvertisementUI>(
        AdvertisementArc::SPRITES,
        AdvertisementArc::SPRITE_NUMBER
    );

    advertisementUI->PreloadMedia();
}