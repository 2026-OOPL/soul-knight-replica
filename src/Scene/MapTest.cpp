#include <memory>

#include "Component/Map/MapPiece.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Scene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Time.hpp"

void MapTest::Initialize() {
    m_MainPlayer = std::make_shared<Player>();
    m_MainPlayer->SetPosition({0.0F, 0.0F});
    m_Player.push_back(m_MainPlayer);

    /*建立碰撞箱*/
    m_TestBlock = std::make_shared<MapPiece>(
        glm::vec2(220.0F, 0.0F), RESOURCE_DIR "/Character/character.png", true);
    m_TestBlock->m_Transform.scale = {2.0F, 2.0F};//長寬放大兩倍
    m_TestBlock->SetColliderSize(m_TestBlock->GetScaledSize());
    this->AddChild(m_TestBlock);
    ////////////

    this->AddChild(std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/Character/Character.png"
        ),
        1
    ));

    LOG_INFO("Game started !");

    m_Pieces.push_back(std::make_shared<BaseRoom>(
        glm::vec2(0, 0)
    ));

    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(-1280, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(1280, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));
    
    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(0, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(-1280, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(1280, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(0, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    m_Pieces.push_back(std::make_shared<MapPiece>(
        glm::vec2(-1280, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));
}

void MapTest::Dispose() {
    if (m_MainPlayer != nullptr) {
        this->RemoveChild(m_MainPlayer);
        m_MainPlayer.reset();
    }

    if (m_TestBlock != nullptr) {
        this->RemoveChild(m_TestBlock);
        m_TestBlock.reset();
    }

    for (const auto &piece : m_Pieces) {
        this->RemoveChild(piece);
    }
}

void MapTest::Update() {

    const glm::vec2 moveDirection = m_MainPlayer->GetMoveIntent();
    if (moveDirection != glm::vec2(0.0F, 0.0F)) {
        const glm::vec2 frameDelta =
            moveDirection * m_PlayerSpeed * Util::Time::GetDeltaTimeMs();

        glm::vec2 nextCoordinate = m_Cooridinate;
        nextCoordinate.x += frameDelta.x;
        if (!WillPlayerCollide(nextCoordinate)) {
            m_Cooridinate.x = nextCoordinate.x;
        }

        nextCoordinate = m_Cooridinate;
        nextCoordinate.y += frameDelta.y;
        if (!WillPlayerCollide(nextCoordinate)) {
            m_Cooridinate.y = nextCoordinate.y;
        }
    }

    for (const auto &piece : m_Pieces) {
        const bool visible = piece->GetVisibilityByCooridinate(m_Cooridinate);
        piece->SetVisible(visible);

        if (visible) {
            piece->SetTransformByCooridinate(m_Cooridinate);
        }

    }

    if (m_TestBlock != nullptr) {
        const bool visible = m_TestBlock->GetVisibilityByCooridinate(m_Cooridinate);
        m_TestBlock->SetVisible(visible);

        if (visible) {
            m_TestBlock->SetTransformByCooridinate(m_Cooridinate);
        }
    }


    Scene::Update();
}

bool MapTest::WillPlayerCollide(const glm::vec2 &nextCoordinate) const {
    // TODO

    return false;
}