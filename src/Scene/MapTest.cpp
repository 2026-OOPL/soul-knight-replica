#include <memory>
#include <string>
#include <glm/vec2.hpp>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Knight.hpp"
#include "Component/UI/PauseUI.hpp"
#include "Component/UI/PlayUI.hpp"
#include "Component/Weapon.hpp"
#include "Generator/MapGenerator.hpp"
#include "MainMenu.hpp"
#include "Util/Animation.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include "Scene/MapTest.hpp"

MapTest::MapTest(
    MapSystemConfig::MapConfig config
) : MapSystem(config) {
    std::shared_ptr<MapGenerator> generator;

    if (config.seed == "") {
        generator = std::make_shared<MapGenerator>(
            config.section == 3,
            config.difficulty
        );
    } else {
        generator = std::make_shared<MapGenerator>(
            config.section == 3,
            config.seed,
            config.difficulty
        );
    }

    this->m_MainPlayer = std::make_shared<Knight>(
        [this] () {return this->GetNearestMonster();}
    );
    
    if (this->m_MainRoom != nullptr) {
        this->m_MainPlayer->SetAbsoluteTranslation(glm::vec2(0.0F, 0.0F));
    }

    this->m_MainPlayer->SetOnWeaponBulletFired(
        [this](std::shared_ptr<Bullet> bullet) {
            this->AddBullet(bullet);
        }
    );

    this->m_MainPlayer->SetAbsoluteScale({0.75F, 0.75F});
    this->AddPlayer(this->m_MainPlayer);

    generator->Generate();
    generator->m_Blueprint->OutputMapGridType();

    this->m_RoomsInScene = generator->GetRooms();
    this->m_GangwaysInScene = generator->GetGangways();
    
    this->AddRooms(this->m_RoomsInScene);
    this->AddGangways(this->m_GangwaysInScene);

    for (const auto &room : this->m_RoomsInScene) {
        if (room == nullptr) {
            continue;
        }

        if (this->m_MainRoom == nullptr &&
            room->GetPurpose() == RoomPurpose::STARTER) {
            this->m_MainRoom = room;
        }
    }

    this->m_PlayUI = std::make_shared<PlayUI>(
        [weakPlayer = std::shared_ptr<Player>(this->m_MainPlayer)]() {
            const std::shared_ptr<Player> player = weakPlayer;
            if (player == nullptr) {
                return PlayerHudState{};
            }

            return player->GetHudState();
        }
    );
    this->AddChild(this->m_PlayUI);

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );

    this->m_AttachCamera->SetScale({2.5F, 2.5F});

    // Level title shown code below

    this->m_LevelIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR "/UI/Gameplay/level_number.png"
        ),
        10
    );

    this->m_LevelIcon->m_Transform.translation = {0, 200.0F};

    this->AddChild(m_LevelIcon);

    this->m_LevelTitle = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/Cubic-Font/Cubic_11.ttf",
            80,
            std::to_string(config.chapter) + "-" + std::to_string(config.section),
            Util::Color(255, 255, 255)
        ),
        10
    );

    this->m_LevelTitle->m_Transform.translation = {11.0F, 195.0F};

    this->AddChild(m_LevelTitle);

    m_SceneStartTime = Util::Time::GetElapsedTimeMs();
}

MapTest::MapTest()
: MapTest(
    MapSystemConfig::MapConfig{
        1 ,
        1,
        GeneratorType::EASY,
        ""
    }
) {
    m_PauseUI = std::make_shared<PauseUI>(
        [this] { this->m_ExitToHome = true; }
    );
}


void MapTest::Update() {
    if (Util::Time::GetElapsedTimeMs() - m_SceneStartTime > 2500) {
        m_LevelTitle->SetVisible(false);
        m_LevelIcon->SetVisible(false);
    }

    // Game pause logic
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_IsPaused = !m_IsPaused;
        
        if (m_IsPaused) {
            m_PauseUI = std::make_shared<PauseUI>(
                [this] { this->m_ExitToHome = true; }
            ); 
            this->AddChild(m_PauseUI);
        } else {
            this->RemoveChild(m_PauseUI);
        }
    }

    if (m_IsPaused && this->m_PauseUI->GetExitSignal()) {
        m_IsPaused = false;
        this->RemoveChild(m_PauseUI);
    }

    if (m_IsPaused) {
        this->m_PauseUI->Update();      
    } else {
        MapSystem::Update();
    }
}

std::shared_ptr<Scene> MapTest::GetRedirection() {
    if (m_ExitToHome) {
        return std::make_shared<MainMenu>();
    }

    return MapSystem::GetRedirection();
}