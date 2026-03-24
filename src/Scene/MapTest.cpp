#include "Scene/MapTest.hpp"

#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

constexpr char kHorizontalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_5x2.png";
constexpr char kVerticalClosedDoorSprite[] = RESOURCE_DIR "/Map/Room/Wall_1x5.png";
constexpr float kPlayerSpawnBelowDoorDistance = 70.0F;
constexpr float kDoorCloseDelayMs = 650.0F;

} // namespace

MapTest::MapTest() : MapSystem() {
    this->m_MainRoom = std::make_shared<BaseRoom>(glm::vec2(0.0F, 0.0F));
    this->m_Pieces.push_back(this->m_MainRoom);

    const glm::vec2 roomCenter = this->m_MainRoom->GetCooridinate();
    const glm::vec2 roomSize = this->m_MainRoom->GetObjectSize();
    const glm::vec2 horizontalDoorColliderSize = {80.0F, 15.0F};
    const glm::vec2 horizontalDoorRenderSize = {80.0F, 32.0F};
    const glm::vec2 verticalDoorColliderSize = {15.0F, 80.0F};
    const glm::vec2 verticalDoorRenderSize = {16.0F,96.0F};
    float offest=15.0F;
    const float bottomDoorCenterY =
        roomCenter.y - roomSize.y / 2.0F + horizontalDoorRenderSize.y / 2.0F+offest;
    const float topDoorCenterY =
        roomCenter.y + roomSize.y / 2.0F - horizontalDoorRenderSize.y / 2.0F+offest-5.0;
    const float leftDoorCenterX =
        roomCenter.x - roomSize.x / 2.0F + verticalDoorRenderSize.x / 2.0F;
    const float rightDoorCenterX =
        roomCenter.x + roomSize.x / 2.0F - verticalDoorRenderSize.x / 2.0F;
    Collision::RoomBoundaryOpenings wallOpenings;
    wallOpenings.top.size = horizontalDoorColliderSize.x;
    wallOpenings.bottom.size = horizontalDoorColliderSize.x;
    wallOpenings.left.size = verticalDoorColliderSize.y;
    wallOpenings.right.size = verticalDoorColliderSize.y;

    const std::vector<Collision::AxisAlignedBox> wallBoxes =
        Collision::BuildRoomBoundaryBoxes(
            roomCenter,
            roomSize,
            this->m_RoomWallThickness,
            wallOpenings
    );
    this->m_CollisionSystem.SetStaticBlockingBoxes(wallBoxes);

    Door::Visuals horizontalDoorVisuals;
    horizontalDoorVisuals.closedIdle = std::make_shared<Util::Image>(kHorizontalClosedDoorSprite);
    Door::Visuals verticalDoorVisuals;
    verticalDoorVisuals.closedIdle = std::make_shared<Util::Image>(kVerticalClosedDoorSprite);

    auto addTestDoor = [this](const std::shared_ptr<Door> &door) {
        this->m_TestDoors.push_back(door);
        this->m_Pieces.push_back(door);
    };

    addTestDoor(std::make_shared<Door>(
        glm::vec2(roomCenter.x, bottomDoorCenterY),
        DoorSide::Bottom,
        horizontalDoorColliderSize,
        horizontalDoorRenderSize,
        horizontalDoorVisuals,
        true
    ));

    addTestDoor(std::make_shared<Door>(
        glm::vec2(roomCenter.x, topDoorCenterY),
        DoorSide::Top,
        horizontalDoorColliderSize,
        horizontalDoorRenderSize,
        horizontalDoorVisuals,
        true
    ));

    addTestDoor(std::make_shared<Door>(
        glm::vec2(leftDoorCenterX, roomCenter.y),
        DoorSide::Left,
        verticalDoorColliderSize,
        verticalDoorRenderSize,
        verticalDoorVisuals,
        true
    ));

    addTestDoor(std::make_shared<Door>(
        glm::vec2(rightDoorCenterX, roomCenter.y),
        DoorSide::Right,
        verticalDoorColliderSize,
        verticalDoorRenderSize,
        verticalDoorVisuals,
        true
    ));

    this->m_MainPlayer = std::make_shared<Player>();
    this->m_MainPlayer->SetPosition({
        roomCenter.x,
        roomCenter.y - roomSize.y / 2.0F - kPlayerSpawnBelowDoorDistance
    });
    this->m_MainPlayer->SetCollisionResolver(
        [this](const Collision::AxisAlignedBox &currentBox, const glm::vec2 &intendedDelta) {
            return this->m_CollisionSystem.ResolveMovement(currentBox, intendedDelta);
        }
    );
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
            for (const auto &door : this->m_TestDoors) {
                if (door != nullptr) {
                    door->Close();
                }
            }
            this->m_DoorCloseDelayRemainingMs = -1.0F;
        }
    }

    this->Scene::Update();

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
}
