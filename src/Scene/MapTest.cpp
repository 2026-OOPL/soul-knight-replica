#include <memory>
#include <stdexcept>
#include <string>
#include <glm/vec2.hpp>
#include <vector>

#include "Common/Enums.hpp"
#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Knight.hpp"
#include "Component/UI/PauseUI.hpp"
#include "Component/UI/PlayUI.hpp"
#include "Component/Weapon.hpp"
#include "GameConfig/GameConfig.hpp"
#include "Generator/MapGenerator.hpp"
#include "Util/BGM.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include "Scene/MapTest.hpp"
#include "MainMenu.hpp"

MapTest::MapTest(
    MapSystemConfig::MapConfig config
) : MapSystem(config) {
    std::shared_ptr<MapGenerator> generator;

    generator = std::make_shared<MapGenerator>(config.difficulty);

    m_MainRoom = generator->GetRooms(RoomPurpose::STARTER).front();

    LOG_INFO("Map genertated by seed {} with result :", generator->GetSeed());
    generator->m_Blueprint->OutputMapGridType();

    // Player initialization
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

    // Setup initial mob wave after player constructed
    this->m_RoomsInScene = generator->GetRooms();
    this->m_GangwaysInScene = generator->GetGangways();
    
    this->AddRooms(this->m_RoomsInScene);
    this->AddGangways(this->m_GangwaysInScene);

    // Gameplay UI initialization
    this->m_PlayUI = std::make_shared<PlayUI>(
        [weakPlayer = std::shared_ptr<Player>(this->m_MainPlayer)]() {
            const std::shared_ptr<Player> player = weakPlayer;
            if (player == nullptr) {
                return PlayerHudState{};
            }

            return player->GetHudState();
        },
        [this]() {
            const std::shared_ptr<BaseRoom> room = this->GetCurrentRoom();
            if (room == nullptr || room->GetPurpose() != RoomPurpose::BOSS) {
                return PlayUI::BossHudState{};
            }

            for (const auto &mob : room->GetMobs()) {
                if (mob == nullptr || mob->IsDead()) {
                    continue;
                }

                return PlayUI::BossHudState{
                    mob->GetCurrentHealth(),
                    mob->GetMaxHealth(),
                    true
                };
            }

            return PlayUI::BossHudState{};
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

    this->m_BGM = std::make_shared<Util::BGM>(
        RESOURCE_DIR"/SFX/Alan Walker - Fade (NCS).mp3"
    );

    switch (config.section) {
        case 1:
            this->m_BGM->LoadMedia(RESOURCE_DIR"/SFX/Tobu - Candyland.mp3");
            break;
        case 2:
            this->m_BGM->LoadMedia(RESOURCE_DIR"/SFX/Alan Walker - The Spectre ( Slowed + Reverb ).mp3");
            break;
        case 3:
            this->m_BGM->LoadMedia(RESOURCE_DIR"/SFX/MONTAGEM ALQUIMIA.mp3");
            break;
    }
    
    this->m_BGM->Play();
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
}

MapTest::~MapTest() {
    this->m_BGM->Pause();
}

void MapTest::Update() {
    if (Util::Time::GetElapsedTimeMs() - m_SceneStartTime > 2500) {
        m_LevelTitle->SetVisible(false);
        m_LevelIcon->SetVisible(false);
    }

    if (m_IsPaused) {
        this->m_PauseUI->Update();
    } else {
        MapSystem::Update();
    }

    // Game pause logic
    if (m_IsPaused) {
        if (this->m_PauseUI->GetExitSignal()) {
            m_IsPaused = false;
            this->RemoveChild(m_PauseUI);
        }
    } else if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_IsPaused = true;

        m_PauseUI = std::make_shared<PauseUI>(
            [this] { this->m_ExitToHome = true; },
            20
        );
        this->AddChild(m_PauseUI);
    }

    this->m_BGM->SetVolume(GameConfig::GetInstance().m_BGMVolume * 128);
}

std::shared_ptr<Scene> MapTest::GetRedirection() {
    if (m_ExitToHome) {
        return std::make_shared<MainMenu>();
    }

    return MapSystem::GetRedirection();
}
