#include <memory>

#include <glm/vec2.hpp>

#include "Common/Random.hpp"
#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/Player/Knight.hpp"
#include "Component/UI/PlayUI.hpp"
#include "Component/Weapon.hpp"
#include "Generator/MapGenerator.hpp"
#include "Scene/MapTest.hpp"

MapTest::MapTest(
    std::shared_ptr<MapGenerator> generator
) : MapSystem() {
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
}

MapTest::MapTest(
    const std::string &seed,
    const GeneratorType type
) : MapTest(
    std::make_shared<MapGenerator>(seed, type)
) {}

MapTest::MapTest()
: MapTest(std::make_shared<MapGenerator>(GeneratorType::EASY)) {}


void MapTest::Update() {
    MapSystem::Update();
}
