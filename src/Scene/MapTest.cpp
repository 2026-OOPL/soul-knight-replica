#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"
#include "Generator/MapGenerator.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

namespace {

void ApplyCameraRecursive(
    const std::shared_ptr<Camera> &camera,
    const std::shared_ptr<Util::GameObject> &object
) {
    if (camera == nullptr || object == nullptr) {
        return;
    }

    camera->SetTransformByCamera(object);

    for (const auto &child : object->GetChildren()) {
        ApplyCameraRecursive(camera, child);
    }
}

} // namespace

MapTest::MapTest() : MapSystem() {
    std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>(
        "I Love OOP"
    );

    generator->Generate();
    this->m_RoomsInScene = generator->GetRooms();
    this->AddRooms(this->m_RoomsInScene);

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

    this->m_MainPlayer = std::make_shared<Player>();
    if (this->m_MainRoom != nullptr) {
        this->m_MainPlayer->SetPosition(this->m_MainRoom->GetAbsoluteCooridinate());
    }

    this->m_MainPlayer->SetCollisionResolver(
        [this](const Collision::AxisAlignedBox &currentBox, const glm::vec2 &intendedDelta) {
            return this->ResolvePlayerMovement(currentBox, intendedDelta);
        }
    );

    this->m_MainPlayer->m_AbsoluteTransform.scale = {.5F, .5F};
    this->m_Players.push_back(this->m_MainPlayer);
    this->UpdateCurrentRoom(this->m_MainPlayer->GetAbsolutePosition());

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );
    this->m_AttachCamera->SetScale({0.5F, 0.5F});

    this->AddChild(this->m_MainPlayer);
}

MapTest::~MapTest() = default;

void MapTest::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::E) &&
        this->m_CurrentRoom != nullptr) {
        this->m_CurrentRoom->OpenAllDoors();
    }

    if (this->m_MainPlayer != nullptr) {
        this->UpdateCurrentRoom(this->m_MainPlayer->GetAbsolutePosition());
    }

    if (this->m_AttachCamera == nullptr) {
        return;
    }

    const std::shared_ptr<IStateful> statefulCamera =
        std::dynamic_pointer_cast<IStateful>(this->m_AttachCamera);
    if (statefulCamera != nullptr) {
        statefulCamera->Update();
    }

    if (this->m_MainPlayer != nullptr) {
        this->m_AttachCamera->SetTransformByCamera(this->m_MainPlayer);
    }

    for (const auto &room : this->m_RoomsInScene) {
        ApplyCameraRecursive(this->m_AttachCamera, room);
    }

    this->Scene::Update();
}
