#include <memory>

#include "Component/IStateful.hpp"
#include "Component/IMapObject.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Scene.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Time.hpp"

MapTest::MapTest() : MapSystem() {
    m_MainPlayer = std::make_shared<Player>();
    m_MainPlayer->SetPosition({0.0F, 0.0F});
    m_Players.push_back(m_MainPlayer);

    // // 建立碰撞箱
    // m_TestBlock = std::make_shared<MapPiece>(
    //     glm::vec2(220.0F, 0.0F), RESOURCE_DIR"/Character/Character.png", true);
    // m_TestBlock->m_Transform.scale = {2.0F, 2.0F};//長寬放大兩倍
    // m_TestBlock->SetColliderSize(m_TestBlock->GetScaledSize());
    // this->AddChild(m_TestBlock);
    
    m_AttachCamera = std::make_shared<TraceCamera>(m_MainPlayer);

    m_Pieces.push_back(std::make_shared<BaseRoom>(
        glm::vec2(0, 0)
    ));
}

void MapTest::Initialize() {

}

void MapTest::Dispose() {
    if (m_MainPlayer != nullptr) {
        this->RemoveChild(m_MainPlayer);
        m_MainPlayer.reset();
    }

    // if (m_TestBlock != nullptr) {
    //     this->RemoveChild(m_TestBlock);
    //     m_TestBlock.reset();
    // }

    for (const auto &piece : m_Pieces) {
        this->RemoveChild(piece);
    }
}

void MapTest::Update() {
    const glm::vec2 moveDirection = m_MainPlayer->GetMoveIntent();

    if (moveDirection != glm::vec2(0.0F, 0.0F)) {
        const glm::vec2 frameDelta =
            moveDirection * m_PlayerSpeed * Util::Time::GetDeltaTimeMs();

        glm::vec2 nextCoordinate = m_MainPlayer->m_Cooridinate;

        nextCoordinate.x += frameDelta.x;
        if (!WillPlayerCollide(nextCoordinate)) {
            m_MainPlayer->m_Cooridinate = nextCoordinate;
        }

        nextCoordinate = m_MainPlayer->m_Cooridinate;
        nextCoordinate.y += frameDelta.y;
        if (!WillPlayerCollide(nextCoordinate)) {
            m_MainPlayer->m_Cooridinate = nextCoordinate;
        }
    }
    
    for (const auto &piece : m_Pieces) {
        std::shared_ptr<IMapObject> object = std::dynamic_pointer_cast<IMapObject>(piece);
        
        if (object) {
            const bool visible = m_AttachCamera->GetVisibility(object);
            piece->SetVisible(visible);

            if (visible) {
                piece->m_Transform = m_AttachCamera->GetObjectTransform(object);
            }
        }

        piece->Draw();
    }

    m_MainPlayer->Draw();

    // Update camera if available
    std::shared_ptr<IStateful> statefulCamera = std::dynamic_pointer_cast<IStateful>(m_AttachCamera);
    if (statefulCamera) {
        statefulCamera->Update();
    }

    Scene::Update();
}

bool MapTest::WillPlayerCollide(const glm::vec2 &nextCoordinate) const {
    // TODO

    return false;
}