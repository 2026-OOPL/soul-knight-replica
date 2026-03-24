#include "Scene/MapTest.hpp"

#include <algorithm>
#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"

namespace {

constexpr float kDoorMarginFromRoom = 70.0F;

} // namespace

MapTest::MapTest() : MapSystem() {
    this->m_MainRoom = std::make_shared<BaseRoom>(glm::vec2(0.0F, 0.0F));
    this->m_MainRoom->m_Transform.scale = {2.0F, 2.0F}; // 放大 2 倍
    this->m_Pieces.push_back(this->m_MainRoom);

    const glm::vec2 roomSize = this->m_MainRoom->GetObjectSize();
    const glm::vec2 roomHalfSize = roomSize / 2.0F;
    const float halfWallThickness = this->m_RoomWallThickness / 2.0F;
    const float maxDoorWidth =
        std::max(0.0F, roomSize.x - (this->m_RoomWallThickness * 2.0F));
    const float maxDoorHeight =
        std::max(0.0F, roomSize.y - (this->m_RoomWallThickness * 2.0F));
    const float minDoorWidth = std::min(110.0F, maxDoorWidth);
    const float minDoorHeight = std::min(110.0F, maxDoorHeight);

    this->m_DoorOpeningSize = {
        std::clamp(roomSize.x * 0.18F, minDoorWidth, maxDoorWidth),
        std::clamp(roomSize.y * 0.18F, minDoorHeight, maxDoorHeight)
    };

    this->m_Doors = {
        std::make_shared<Door>(
            glm::vec2(0.0F, roomHalfSize.y - halfWallThickness),
            DoorSide::Top,
            glm::vec2(this->m_DoorOpeningSize.x, this->m_RoomWallThickness),
            true
        ),
        std::make_shared<Door>(
            glm::vec2(roomHalfSize.x - halfWallThickness, 0.0F),
            DoorSide::Right,
            glm::vec2(this->m_RoomWallThickness, this->m_DoorOpeningSize.y),
            true
        ),
        std::make_shared<Door>(
            glm::vec2(0.0F, -roomHalfSize.y + halfWallThickness),
            DoorSide::Bottom,
            glm::vec2(this->m_DoorOpeningSize.x, this->m_RoomWallThickness),
            true
        ),
        std::make_shared<Door>(
            glm::vec2(-roomHalfSize.x + halfWallThickness, 0.0F),
            DoorSide::Left,
            glm::vec2(this->m_RoomWallThickness, this->m_DoorOpeningSize.y),
            true
        )
    };

    for (const auto &door : this->m_Doors) {
        this->m_Pieces.push_back(door);
    }

    this->m_MainPlayer = std::make_shared<Player>();
    this->m_MainPlayer->SetPosition({
        0.0F,
        -roomHalfSize.y - kDoorMarginFromRoom
    });
    this->m_Players.push_back(this->m_MainPlayer);

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );

    this->m_CollisionSystem = std::make_shared<Collision::CollisionSystem>();
    this->m_CollisionSystem->SetBlockingBoxProvider([this]() {
        return Collision::BuildWallBoxes(this->m_Pieces);
    });
    this->m_CollisionSystem->SetStaticBlockingBoxes(
        Collision::BuildRoomBoundaryBoxes(
            this->m_MainRoom->GetCooridinate(),
            roomSize,
            this->m_RoomWallThickness,
            this->m_DoorOpeningSize
        )
    );

    this->m_MainPlayer->SetCollisionResolver(
        [this](
            const Collision::AxisAlignedBox &currentBox,
            const glm::vec2 &intendedDelta
        ) {
            return this->m_CollisionSystem->ResolveMovement(currentBox, intendedDelta);
        }
    );

    this->AddChild(this->m_MainPlayer);

    for (const auto &piece : this->m_Pieces) {
        this->AddChild(piece);
    }
}

MapTest::~MapTest() {
    this->RemoveChild(this->m_MainPlayer);

    for (const auto &piece : this->m_Pieces) {
        this->RemoveChild(piece);
    }
}

void MapTest::Update() {
    this->Scene::Update();

    if (!this->m_HasPlayerEnteredRoom && this->IsPlayerInsideRoom()) {
        this->m_HasPlayerEnteredRoom = true;
        this->CloseAllDoors();
    }

    if (this->m_AttachCamera == nullptr) {
        return;
    }

    std::shared_ptr<IStateful> statefulCamera =
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
}

void MapTest::CloseAllDoors() {
    for (const auto &door : this->m_Doors) {
        if (door != nullptr) {
            door->Close();
        }
    }
}
