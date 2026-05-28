#include <cstdlib>
#include <memory>

#include "Common/MapObject.hpp"
#include "Component/Casting/CastingText.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "MainMenu.hpp"
#include "Scene/CastingScene.hpp"
#include "Util/BGM.hpp"
#include "Util/Input.hpp"
#include "Util/SFX.hpp"
#include "Util/Time.hpp"

namespace {
    const int SCROLLING_END_POSITION = -1900;
};

CastingScene::CastingScene() {
    m_AttachedCamera = std::make_shared<TraceCamera>(
        nullptr,
        std::make_shared<EaseOutQubicCurve>()
    );

    m_AttachedCamera->SetCooridinate(glm::vec2(0, 360));
    
    this->AddChild(std::make_shared<CastingText>(
        "奧奧批欸特貳零貳陸",
        glm::vec2(-57, 0),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Soul Knight Replica",
        glm::vec2(0, -40),
        36
    ));

    this->AddChild(std::make_shared<CastingText>(
        "原作 | Original",
        glm::vec2(0, -140),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "元氣騎士 by 涼屋遊戲",
        glm::vec2(0, -190)
    ));
    
    this->AddChild(std::make_shared<CastingText>(
        "遊戲總監 | Game Director",
        glm::vec2(0, -290),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "YFHD-osu, Kenilikecats",
        glm::vec2(0, -330)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "執行製作人 | Executive Producer",
        glm::vec2(0, -430),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "YFHD-osu, Kenilikecats",
        glm::vec2(0, -480)
    ));
    
    this->AddChild(std::make_shared<CastingText>(
        "Codex 5.5, Gemini 3 Pro, Claude Sonnet 4.6",
        glm::vec2(0, -530)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "美術與視覺 | Art Director",
        glm::vec2(0, -630),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "圖片版權皆屬於凉屋游戏",
        glm::vec2(0, -680)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "音樂與音效 | Art Director",
        glm::vec2(0, -780),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Alan Walker - The Spectre",
        glm::vec2(0, -830)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Alan Walker - Just the way you are",
        glm::vec2(0, -880)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Alan Walker - Today is the best day for OOP demo ",
        glm::vec2(0, -930)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Alan Walker - My Way",
        glm::vec2(0, -980)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "特別致謝 | Special Thanks",
        glm::vec2(0, -1080),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "天、地、衣食父母、國立臺北科技大學",
        glm::vec2(0, -1130)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "OpenAI, Alphabet, Anthropic, Intel, Apple",
        glm::vec2(0, -1180)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Stack Overflow, Reddit 網友, Gcc, Cmake",
        glm::vec2(0, -1230)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "陽光、空氣、H₂O、陽光與仙人掌",
        glm::vec2(0, -1280)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "特別內部供暖支援 | Special Internal Heating Support",
        glm::vec2(0, -1380),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Acer Swift Go 14 (2024)",
        glm::vec2(0, -1430)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Gigabyte 鐵磚",
        glm::vec2(0, -1480)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "感謝您的遊玩，不要當我們拜託～",
        glm::vec2(0, SCROLLING_END_POSITION)
    ));

    std::shared_ptr<Util::BGM> m_Music = std::make_shared<Util::BGM>(
        RESOURCE_DIR"/SFX/Elektronomia - Sky High Intro.mp3"
    );

    m_Music->FadeIn(1000, 0);
    m_Music->Play();
    
    m_SceneStartTime = Util::Time::GetElapsedTimeMs();
}

CastingScene::~CastingScene() {
    
}

void CastingScene::Update() {
    if (!m_SwitchBGM && Util::Time::GetElapsedTimeMs() - m_SceneStartTime >= 4563) {
        m_Music->Pause();
        std::shared_ptr<Util::BGM> m_Music = std::make_shared<Util::BGM>(
            RESOURCE_DIR"/SFX/Elektronomia - Sky High Loop.mp3"
        );
        m_Music->Play();
        m_SwitchBGM = true;
    }

    for (auto const &i : this->GetChildren()) {
        std::shared_ptr<IStateful> stateful = std::dynamic_pointer_cast<IStateful>(i);
        
        if (stateful) {
            stateful->Update();
        }
        
        std::shared_ptr<MapObject> mapObject = std::dynamic_pointer_cast<MapObject>(i);
        
        if (m_AttachedCamera && mapObject) {
            m_AttachedCamera->SetTransformByCamera(i);
        }
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
        m_ScrollingSpeed = 10.0F;
    } else {
        m_ScrollingSpeed = 1.0F;
    }

    glm::vec2 pos = m_AttachedCamera->GetCooridinate();

    if (pos.y > SCROLLING_END_POSITION && abs(pos.y - m_ScrollingSpeed) < abs(SCROLLING_END_POSITION)) {
        m_AttachedCamera->SetCooridinate(pos - glm::vec2(0.0F, m_ScrollingSpeed));
    } else {
        m_AttachedCamera->SetCooridinate(glm::vec2(0.0F, SCROLLING_END_POSITION));
        m_CastingEndTime = (m_CastingEndTime < 0) ? Util::Time::GetElapsedTimeMs() : m_CastingEndTime;
    }

    float castEndElapsed = Util::Time::GetElapsedTimeMs() - m_CastingEndTime;
    if (m_CastingEndTime > 0 &&  castEndElapsed > 2000 && castEndElapsed < 3000) {
        m_Music->FadeOut(1000);
    } else if (m_CastingEndTime > 0 && Util::Time::GetElapsedTimeMs() - m_CastingEndTime > 3000) {
        m_SceneRedirection = std::make_shared<MainMenu>();
    }

    Scene::Update();
}

std::shared_ptr<Scene> CastingScene::GetRedirection() {
    return this->m_SceneRedirection;
}