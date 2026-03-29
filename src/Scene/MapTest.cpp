#include <glm/fwd.hpp>
#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"
#include "Component/Player/Knight.hpp"
#include "Component/Weapon/Weapon.hpp"
#include "Generator/MapGenerator.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

MapTest::MapTest() : MapSystem() {
    std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>(
        "I Love OOP"
    );

    generator->Generate();
    this->m_RoomsInScene = generator->GetRooms();
    this->m_GangwaysInScene = generator->GetGangways();
    this->AddRooms(this->m_RoomsInScene);
    this->AddGangways(this->m_GangwaysInScene);

    for (const auto &room : this->m_RoomsInScene) {
        if (room == nullptr) {
            continue;
        }

        this->AddChild(room);

        if (this->m_MainRoom == nullptr &&
            room->GetPurpose() == RoomPurpose::STARTER) {
            this->m_MainRoom = room;
        }
    }

    for (const auto &gangway : this->m_GangwaysInScene) {
        if (gangway != nullptr) {
            this->AddChild(gangway);
        }
    }

    this->m_MainPlayer = std::make_shared<Knight>();
    if (this->m_MainRoom != nullptr) {
        this->m_MainPlayer->SetAbsoluteTranslation(glm::vec2(0 ,0));
    }

    this->m_MainPlayer->SetCollisionResolver(
        [this](const Collision::AxisAlignedBox &currentBox, const glm::vec2 &intendedDelta) {
            return this->ResolvePlayerMovement(currentBox, intendedDelta);
        }
    );

    // Set call function while player shots a bullet
    if (this->m_MainPlayer->GetWeapon() != nullptr) {
        this->m_MainPlayer->GetWeapon()->SetOnBulletFired(
            [this](std::shared_ptr<Bullet> bullet) {
                this->AddBullet(bullet);
            }
        );
    }

    this->m_MainPlayer->SetAbsoluteScale({.75F, .75F});
    if (this->m_MainPlayer != nullptr) {
        this->m_Players.push_back(this->m_MainPlayer);
        this->AddChild(this->m_MainPlayer);
    }

    this->UpdateCurrentRoom(this->m_MainPlayer->GetAbsoluteTranslation());

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );

    this->m_AttachCamera->SetScale({2.5F, 2.5F});

}

MapTest::~MapTest() = default;

void MapTest::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::E) &&
        this->m_CurrentRoom != nullptr) {
        this->m_CurrentRoom->OpenAllDoors();
    }

    if (this->m_MainPlayer != nullptr) {
        this->UpdateCurrentRoom(this->m_MainPlayer->GetAbsoluteTranslation());
    }
    
    // 呼叫父類別 MapSystem 的 Update，它會自動處理好所有相機轉換與 Scene::Update()
    MapSystem::Update(); 
}
