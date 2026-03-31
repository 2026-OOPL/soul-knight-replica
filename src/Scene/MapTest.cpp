#include <memory>

#include <glm/fwd.hpp>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/Player/Knight.hpp"
#include "Component/Weapon.hpp"
#include "Generator/MapGenerator.hpp"
#include "Scene/MapTest.hpp"

MapTest::MapTest()
    : MapSystem() {
    std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>("I Love OOP");

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

    this->m_MainPlayer = std::make_shared<Knight>();
    if (this->m_MainRoom != nullptr) {
        this->m_MainPlayer->SetAbsoluteTranslation(glm::vec2(0.0F, 0.0F));
    }

    if (this->m_MainPlayer->GetWeapon() != nullptr) {
        this->m_MainPlayer->GetWeapon()->SetOnBulletFired(
            [this](std::shared_ptr<Bullet> bullet) {
                this->AddBullet(bullet);
            }
        );
    }

    this->m_MainPlayer->SetAbsoluteScale({0.75F, 0.75F});
    this->AddPlayer(this->m_MainPlayer);

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );
    this->m_AttachCamera->SetScale({2.5F, 2.5F});
}

MapTest::~MapTest() = default;

void MapTest::Update() {
    const std::shared_ptr<BaseRoom> currentRoom = this->GetCurrentRoom();
    if (Util::Input::IsKeyDown(Util::Keycode::E) && currentRoom != nullptr) {
        currentRoom->OpenAllDoors();
    }

    MapSystem::Update();
}
