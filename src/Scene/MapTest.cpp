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
#include "Scene/Gameover.hpp"
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

    generator = std::make_shared<MapGenerator>(config.info.difficulty);

    m_MainRoom = generator->GetRooms(RoomPurpose::STARTER).front();

    LOG_INFO("Map genertated by seed {} with result :", generator->GetSeed());
    generator->m_Blueprint->OutputMapGridType();

    // Player initialization
    this->m_MainPlayer = std::make_shared<Knight>(
        config.playerInfo,
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
    this->m_PlayUI->SetZIndex(8);
    this->AddChild(this->m_PlayUI);

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );

    this->m_AttachCamera->SetScale({3.0F, 3.0F});

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
            std::to_string(config.info.chapter) + "-" + std::to_string(config.info.section),
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

    switch (config.info.section) {
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
    MapSystemConfig::MapConfig{}
) {}

MapTest::~MapTest() {
    this->m_BGM->Pause();
}

void MapTest::Update() {
    // Level title display logic
    if (m_LevelTitle && m_LevelIcon && Util::Time::GetElapsedTimeMs() - m_SceneStartTime > 2500) {
        this->RemoveChild(m_LevelTitle);
        this->RemoveChild(m_LevelIcon);
        m_LevelTitle = nullptr;
        m_LevelIcon = nullptr;
    }

    switch (m_MapState) {
        case MapState::PLAYING:
            MapSystem::Update();
            break;
        case MapState::PAUSED:
            if (this->m_PauseUI) {
                this->m_PauseUI->Update();

                bool exitSignal = this->m_PauseUI->GetExitSignal();

                this->SetPauseUIVisible(!exitSignal);
            }
            break;
        case MapState::RESPAWNING:
            if (this->m_RespawnUI) {
                this->m_RespawnUI->Update();

                if (this->m_RespawnUI) {
                    bool exitSignal = this->m_RespawnUI->GetExitSignal();
                    this->SetRespawnUIVisible(!exitSignal);
                }
            }
            break;
    }

    if (m_MapState != MapState::RESPAWNING && 
        Util::Input::IsKeyUp(Util::Keycode::ESCAPE)
    ) {
        m_MapState = (m_MapState == MapState::PAUSED) ? MapState::PLAYING : MapState::PAUSED;
        this->SetPauseUIVisible(m_MapState == MapState::PAUSED);
    }

    this->m_BGM->SetVolume(GameConfig::GetInstance().m_BGMVolume * 128);

    std::shared_ptr<Player> player = this->GetPlayers().front();

    if (player->IsDead() && m_MapState != MapState::RESPAWNING) {
        this->m_BGM->Pause();
        this->m_MapState = MapState::RESPAWNING;
        this->SetRespawnUIVisible(true);
    }

}

std::shared_ptr<Scene> MapTest::GetRedirection() {
    if (m_ExitToHome) {
        return std::make_shared<MainMenu>();
    }

    return MapSystem::GetRedirection();
}

void MapTest::SetPauseUIVisible(bool visible) {
    if (visible) {
        if (this->m_PauseUI != nullptr) {
            return;
        }

        this->m_PauseUI = std::make_shared<PauseUI>(
            [this] { this->m_ExitToHome = true; }
        );

        this->m_PauseUI->SetZIndex(this->GetZIndex() + 20);

        this->AddChild(this->m_PauseUI);
    } else {
        if (this->m_PauseUI == nullptr) {
            return;
        }

        this->RemoveChild(this->m_PauseUI);
        this->m_PauseUI = nullptr;
    }
}

void MapTest::SetRespawnUIVisible(bool visible) {
    if (visible) {
        if (this->m_RespawnUI != nullptr) {
            return;
        }

        this->m_RespawnUI = std::make_shared<RespawnUI>(
            [this] { this->m_RedirectScene = std::make_shared<GameoverScene>(); },
            [this] { this->RespawnPlayer(); }
        );

        this->m_RespawnUI->SetZIndex(this->GetZIndex() + 10);

        this->AddChild(this->m_RespawnUI);
    } else {
        if (this->m_RespawnUI == nullptr) {
            return;
        }

        this->RemoveChild(this->m_RespawnUI);
        this->m_RespawnUI = nullptr;
    }
}

void MapTest::RespawnPlayer() {
    this->m_BGM->Play();
    
    this->m_MainPlayer->SetCurrentHealth(this->m_MainPlayer->GetMaxHealth());
    this->m_MainPlayer->SetCurrentShield(this->m_MainPlayer->GetMaxShield());
    this->m_MainPlayer->SetCurrentAmmo(this->m_MainPlayer->GetMaxAmmo());

    m_MapState = MapState::PLAYING;

    this->SetRespawnUIVisible(false);
}