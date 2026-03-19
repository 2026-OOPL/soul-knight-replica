#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Scene.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Time.hpp"

MapTest::MapTest() : MapSystem() {
    m_MainPlayer = std::make_shared<Player>();
    m_MainPlayer->SetPosition({0.0F, 0.0F});
    m_Players.push_back(m_MainPlayer);
    
    m_AttachCamera = std::make_shared<TraceCamera>(
        m_MainPlayer, std::make_shared<EaseOutQubicCurve>()
    );

    m_Pieces.push_back(std::make_shared<BaseRoom>(
        glm::vec2(0, 0)
    ));

    m_CollisionSystem = std::make_shared<Collision::CollisionSystem>();
    m_CollisionSystem->SetBlockingBoxProvider([this]() {
        return Collision::BuildWallBoxes(m_Pieces);
    });

    if (!m_Pieces.empty()) {
        const std::shared_ptr<MapPiece> room = m_Pieces.front();

        m_CollisionSystem->SetStaticBlockingBoxes(
            Collision::BuildRoomBoundaryBoxes(
                room->GetCooridinate(),
                room->GetObjectSize(),
                25.0F
            )
        );
    }

    m_MainPlayer->SetCollisionResolver(
        [this](
            const Collision::AxisAlignedBox &currentBox,
            const glm::vec2 &intendedDelta
        ) {
            return m_CollisionSystem->ResolveMovement(currentBox, intendedDelta);
        }
    );

    this->AddChild(m_MainPlayer);

    for (auto const& i : m_Pieces) {
        this->AddChild(i);
    }
}

MapTest::~MapTest() {
    this->RemoveChild(m_MainPlayer);

    for (auto const& i : m_Pieces) {
        this->RemoveChild(i);
    }
}

void MapTest::Initialize() {
}

void MapTest::Dispose() {
}

void MapTest::Update() {    
    for (const auto &piece : m_Pieces) {
        m_AttachCamera->SetTransformByCamera(piece);
    }

    m_MainPlayer->Update();

    // Update camera if available
    std::shared_ptr<IStateful> statefulCamera = std::dynamic_pointer_cast<IStateful>(m_AttachCamera);
    if (statefulCamera) {
        statefulCamera->Update();
    }

    Scene::Update();
}
