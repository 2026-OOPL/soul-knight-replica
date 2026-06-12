#include <memory>
#include <string>

#include "Scene/Gameover.hpp"
#include "Component/Player/Knight.hpp"
#include "GameConfig/GameConfig.hpp"
#include "MainMenu.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"

namespace {
    constexpr int kAnimationDuration = 2000;
    constexpr int kAnimationStartDelay = 1000;
    constexpr int kAnimationEndDelay = 1000;

    constexpr int kAnimationStartPositionX = -540;
    constexpr int kAnimationEndPositionX = 505;
}

GameoverScene::GameoverScene(MapSystemConfig::MapConfig config) : Scene() {
    this->m_Config = config;
    this->m_AnimationEndPositionX = GetAnimationEndPositionX(m_Config.info);
    
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

    // Music setup
    if (this->m_Config.info.section == 3 && this->m_Config.info.chapter == 3) {
        this->m_BGM = std::make_shared<Util::BGM>(
            RESOURCE_DIR"/SFX/DJ R4 - 67.mp3"
        );
        this->m_BGM->LoadMedia(RESOURCE_DIR"/SFX/DJ R4 - 67.mp3");
    } else {
        this->m_BGM = std::make_shared<Util::BGM>(
            RESOURCE_DIR"/SFX/Nokia Arabic Ringtone.mp3"
        );
        this->m_BGM->LoadMedia(RESOURCE_DIR"/SFX/Nokia Arabic Ringtone.mp3");
    }

    this->m_BGM->SetVolume(GameConfig::GetInstance().m_BGMVolume * 128);

    // Player character indicator
    this->m_Knight = std::make_shared<Knight>(
        MapSystemConfig::PlayerInfo {
            KnightPlayer::MAX_HEALTH,
            KnightPlayer::MAX_SHIELD,
            KnightPlayer::MAX_AMMO
        },
        [] () { return nullptr; }
    );

    this->m_Knight->m_Transform.translation = glm::vec2(kAnimationStartPositionX, 150.0F);
    this->m_Knight->m_Transform.scale = glm::vec2(3.0F, 3.0F);
    this->m_Knight->SetWeapon(nullptr);
    this->AddChild(this->m_Knight);

    this->m_LevelText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/Font/Cubic-Font/Cubic_11.ttf",
        36,
        "1-1",
        Util::Color(255, 255, 255)
    );

    this->m_LevelTextObject = std::make_shared<Util::GameObject>(
        this->m_LevelText,
        10
    );
    this->m_LevelTextObject->m_Transform.translation = glm::vec2(kAnimationStartPositionX+25, 70.0F);

    this->AddChild(this->m_LevelTextObject);

    // Set the scene start time for animation timing
    m_SceneStartTime = Util::Time::GetElapsedTimeMs();

    this->m_Ghostking = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Animation>(
            std::vector<std::string>{
                RESOURCE_DIR"/Boss/GhostKing/GhostKing_stand_0.png",
                RESOURCE_DIR"/Boss/GhostKing/GhostKing_stand_1.png",
                RESOURCE_DIR"/Boss/GhostKing/GhostKing_stand_2.png"
            },
            true,
            20,
            true,
            0,
            false
        ),
        10
    );
    this->m_Ghostking->m_Transform.translation = {-400, -50};
    this->m_Ghostking->m_Transform.scale = {2.0F, 2.0F};
    this->AddChild(this->m_Ghostking);

    this->m_TextGhostking = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
            36,
            (config.info.chapter >= 2) ? "已通過" : "未通過",
            Util::Color(255, 255, 255),
            true
        ),
        10
    );
    this->m_TextGhostking->m_Transform.translation = {-400, -150};
    this->AddChild(this->m_TextGhostking);

    this->m_VitaminCMecha = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Animation>(
            std::vector<std::string>{
                RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_0.png",
                RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_1.png",
                RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_2.png",
                RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_3.png"
            },
            true,
            20,
            true,
            0,
            false
        ),
        10
    );
    this->m_VitaminCMecha->m_Transform.translation = {-125, -50};
    this->AddChild(this->m_VitaminCMecha);
    
    m_TextGhostking = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
            36,
            (config.info.chapter >= 3) ? "已通過" : "未通過",
            Util::Color(255, 255, 255),
            true
        ),
        10
    );
    this->m_TextGhostking->m_Transform.translation = {-125, -150};
    this->AddChild(m_TextGhostking);
    
    this->m_ZulanInRuins = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Animation>(
            std::vector<std::string>{
                RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsBanner.png"
            },
            true,
            20,
            true,
            0,
            false
        ),
        10
    );
    this->m_ZulanInRuins->m_Transform.scale = {2.0F, 2.0F};
    this->m_ZulanInRuins->m_Transform.translation = {150, -50};
    this->AddChild(this->m_ZulanInRuins);

    this->m_TextZulanInRuins = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
            36,
            (config.info.chapter >= 3 && config.info.section >= 3) ? "已通過" : "未通過",
            Util::Color(255, 255, 255),
            true
        ),
        10
    );
    this->m_TextZulanInRuins->m_Transform.translation = {150, -150};
    this->AddChild(m_TextZulanInRuins);
}

void GameoverScene::Update() {
    if (!m_SwitchBGM) {
        this->m_BGM->Play();
        m_SwitchBGM = true;
    }

    if (this->m_BGM) {
        this->m_BGM->SetVolume(GameConfig::GetInstance().m_BGMVolume * 128);
    }

    MoveCharacterAnimation();

    Scene::Update();
}

std::shared_ptr<Scene> GameoverScene::GetRedirection() {
    if (this->m_SceneRedirection != nullptr) {
        return this->m_SceneRedirection;
    }

    return Scene::GetRedirection();
}

void GameoverScene::MoveCharacterAnimation() {
    Util::ms_t duration = Util::Time::GetElapsedTimeMs() - m_SceneStartTime;
    if (duration < kAnimationStartDelay) {
        this->m_Knight->m_Transform.translation = glm::vec2(kAnimationStartPositionX, 150.0F);
    } else if (duration < kAnimationStartDelay + kAnimationDuration) {
        float t = (duration - kAnimationStartDelay) / static_cast<float>(kAnimationDuration);
        float p = kAnimationStartPositionX + (m_AnimationEndPositionX - kAnimationStartPositionX) * t;
        this->m_LevelText->SetText(this->GetLevelTextByPosition(p));
        this->m_Knight->m_Transform.translation = glm::vec2(p, 150.0F);
        m_LevelTextObject->m_Transform.translation = glm::vec2(p+25, 70.0F);
    } else {
        this->m_Knight->m_Transform.translation = glm::vec2(m_AnimationEndPositionX, 150.0F);

        if (m_Config.info.chapter != 3 || m_Config.info.section != 3) {
            this->m_Knight->SetCurrentHealth(0);
        }

        this->m_Knight->Update();
    }    
}

std::string GameoverScene::GetLevelTextByPosition(float positionX) const {
    if (positionX >= this->GetAnimationEndPositionX({ 3, 3}) - 50) {
        return "3-3";
    } else if (positionX >= this->GetAnimationEndPositionX({ 3, 2}) - 50) {
        return "3-2";
    } else if (positionX >= this->GetAnimationEndPositionX({ 3, 1}) - 50) {
        return "3-1";
    } else if (positionX >= this->GetAnimationEndPositionX({ 2, 3}) - 50) {
        return "2-3";
    } else if (positionX >= this->GetAnimationEndPositionX({ 2, 2}) - 50) {
        return "2-2";
    } else if (positionX >= this->GetAnimationEndPositionX({ 2, 1}) - 50) {
        return "2-1";
    } else if (positionX >= this->GetAnimationEndPositionX({ 1, 3}) - 50) {
        return "1-3";
    } else if (positionX >= this->GetAnimationEndPositionX({ 1, 2}) - 50) {
        return "1-2";
    } else if (positionX >= this->GetAnimationEndPositionX({ 1, 1}) - 50) {
        return "1-1";
    } else {
        return "1-1";
    }
}

float GameoverScene::GetAnimationEndPositionX(MapSystemConfig::MapInfo info) const {
    if (info.chapter == 3 && info.section == 3) {
        return kAnimationEndPositionX;
    }

    float offset = (float) kAnimationStartPositionX;

    offset += (info.chapter - 1) * 605.0F / 1.7F;
    offset += (info.section)     * 153.0F / 1.7F;
    
    return offset;
}