#include "Scene/LevelChoose.hpp"
#include "Common/Random.hpp"
#include "Generator/MapGenerator.hpp"
#include "MainMenu.hpp"
#include "Scene/MapTest.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"
#include <memory>

LevelSwitch::LevelSwitch() {
    m_LoadingText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
            32,
            "Loading Now..."
        ), 3
    );

    this->AddChild(m_LoadingText);

    m_SceneStartTime = Util::Time::GetElapsedTimeMs();
}

void LevelSwitch::Update() {
    Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (now - this->m_SceneStartTime > 1000) {
        std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>(GeneratorType::EASY);
        m_Redirect_Scene = std::make_shared<MapTest>(generator);
    }
}

std::shared_ptr<Scene> LevelSwitch::GetRedirection() {
    return this->m_Redirect_Scene;
}