#include <memory>

#include "Scene/LevelChoose.hpp"
#include "Generator/MapGenerator.hpp"
#include "Scene/MapTest.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"

LevelSwitch::LevelSwitch(MapSystemConfig::MapConfig config) {
    m_LoadingText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
            32,
            "Loading Now..."
        ), 3
    );

    this->AddChild(m_LoadingText);

    m_SceneStartTime = Util::Time::GetElapsedTimeMs();

    this->m_MapConfig = config;
}

void LevelSwitch::Update() {
    Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (now - this->m_SceneStartTime > 1000) {
        
        if (m_MapConfig.section < 3) {
            m_MapConfig.section += 1;
        }  else {
            m_MapConfig.section = 1;
            m_MapConfig.chapter += 1;
        }

        switch(m_MapConfig.section) {
            case 1:
                m_MapConfig.difficulty = GeneratorType::EASY;
                break;
            case 2:
                m_MapConfig.difficulty = GeneratorType::MEDIUM;
                break;
            case 3:
                m_MapConfig.difficulty = GeneratorType::HARD;
                break;
        }
        
        m_Redirect_Scene = std::make_shared<MapTest>(m_MapConfig);
    }
}

std::shared_ptr<Scene> LevelSwitch::GetRedirection() {
    return this->m_Redirect_Scene;
}