#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"
#include "Scene/MapTest.hpp"
#include "Generator/MapBlueprint.hpp"
#include "Generator/MapGenerator.hpp"
#include "Util/Time.hpp"

namespace {

constexpr float kPlayerSpawnBelowDoorDistance = 70.0F;
constexpr float kDoorCloseDelayMs = 650.0F;

} // namespace

MapTest::MapTest() : MapSystem() {
    std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>(
        "I Love OOP"
    );

    generator->Generate();

    std::vector<RoomAssembly> roomAssembly = generator->GetRoomAssembly();

    for (auto const &i : roomAssembly) {
        this->AddMapPieces(i.GetPieces());
        this->m_CollisionSystem.AddStaticBlockingBoxes(i.GetStaticWallBoxes());
    }
    
    this->m_MainPlayer = std::make_shared<Player>();

    this->m_MainPlayer->SetPosition({0, 0});
    // this->m_MainPlayer->SetPosition(
    //     this->m_MainRoomAssembly->GetSuggestedBottomSpawn(kPlayerSpawnBelowDoorDistance)
    // );
    this->m_MainPlayer->SetCollisionResolver(
        [this](const Collision::AxisAlignedBox &currentBox, const glm::vec2 &intendedDelta) {
            return this->m_CollisionSystem.ResolveMovement(currentBox, intendedDelta);
        }
    );

    m_MainPlayer->m_AbsoluteTransform.scale = {.5, .5};

    this->m_CollisionSystem.SetBlockingBoxProvider(
        [this]() {
            return Collision::BuildWallBoxes(this->m_Pieces);
        }
    );
    this->m_Players.push_back(this->m_MainPlayer);

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );

    this->m_AttachCamera->SetScale({3, 3});

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
    if (!this->m_HasPlayerEnteredMainRoom && this->IsPlayerInsideRoom()) {
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
