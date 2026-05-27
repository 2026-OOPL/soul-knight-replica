#include <memory>

#include "Common/MapObject.hpp"
#include "Component/Casting/CastingText.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "MainMenu.hpp"
#include "Scene/CastingScene.hpp"
#include "Util/Time.hpp"

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
        "版權皆屬於凉屋游戏",
        glm::vec2(0, -680)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "特別致謝 | Special Thanks",
        glm::vec2(0, -780),
        20
    ));

    this->AddChild(std::make_shared<CastingText>(
        "馬英九、蔡英文、台灣",
        glm::vec2(0, -830)
    ));

    this->AddChild(std::make_shared<CastingText>(
        "Thank you for playing",
        glm::vec2(0, -1300)
    ));
    
}

void CastingScene::Update() {
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

    glm::vec2 pos = m_AttachedCamera->GetCooridinate();

    if (pos.y > -1300.0F) {
        m_AttachedCamera->SetCooridinate(pos - glm::vec2(0.0F, 1.0F));
    } else {
        m_CastingEndTime = (m_CastingEndTime < 0) ? Util::Time::GetElapsedTimeMs() : m_CastingEndTime;
    }

    if (m_CastingEndTime > 0 && Util::Time::GetElapsedTimeMs() - m_CastingEndTime > 3000) {
        m_SceneRedirection = std::make_shared<MainMenu>();
    }

    Scene::Update();
}

std::shared_ptr<Scene> CastingScene::GetRedirection() {
    return this->m_SceneRedirection;
}