#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"
#include "Generator/MapGenerator.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Time.hpp"

namespace {

constexpr float kDoorCloseDelayMs = 650.0F;

} // namespace

MapTest::MapTest() : MapSystem() {
    std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>(
        "I Love OOP"
    );

    generator->Generate();

    this->m_RoomAssemblies = generator->GetRoomAssembly();

    for (const auto &roomAssembly : this->m_RoomAssemblies) {
        if (roomAssembly == nullptr) {
            continue;
        }

        this->AddMapPieces(roomAssembly->GetPieces());
        this->AddRoom(roomAssembly->GetRoom());

        if (this->m_MainRoomAssembly == nullptr &&
            roomAssembly->GetPurpose() == RoomPurpose::STARTER) {
            this->m_MainRoomAssembly = roomAssembly;
        }
    }

    this->m_MainPlayer = std::make_shared<Player>();

    if (this->m_MainRoomAssembly != nullptr) {
        this->m_MainPlayer->SetPosition(
            this->m_MainRoomAssembly->GetRoom()->GetAbsoluteCooridinate()
        );
    } else {
        this->m_MainPlayer->SetPosition({0.0F, 0.0F});
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

    this->m_AttachCamera->SetScale({0.5, 0.5});

    if (this->m_MainPlayer != nullptr) {
        this->AddChild(this->m_MainPlayer);
    }

    for (const auto &piece : this->m_Pieces) {
        if (piece != nullptr) {
            this->AddChild(piece);
        }
    }
}

MapTest::~MapTest() = default;

void MapTest::Update() {
    if (this->m_MainPlayer != nullptr) {
        this->UpdateCurrentRoom(this->m_MainPlayer->GetAbsolutePosition());
    }

    if (!this->m_HasPlayerEnteredMainRoom &&
        this->m_MainRoomAssembly != nullptr &&
        this->m_MainPlayer != nullptr &&
        this->m_MainRoomAssembly->GetRoom()->IsPlayerInside(
            this->m_MainPlayer->GetAbsolutePosition()
        )) {
        this->m_HasPlayerEnteredMainRoom = true;
        this->m_DoorCloseDelayRemainingMs = kDoorCloseDelayMs;
    }

    if (this->m_DoorCloseDelayRemainingMs >= 0.0F) {
        this->m_DoorCloseDelayRemainingMs -= Util::Time::GetDeltaTimeMs();

        if (this->m_DoorCloseDelayRemainingMs <= 0.0F) {
            if (this->m_MainRoomAssembly != nullptr) {
                this->m_MainRoomAssembly->CloseAllDoors();
            }
            this->m_DoorCloseDelayRemainingMs = -1.0F;
        }
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

    for (const auto &piece : this->m_Pieces) {
        this->m_AttachCamera->SetTransformByCamera(piece);
    }

    this->Scene::Update();
}
