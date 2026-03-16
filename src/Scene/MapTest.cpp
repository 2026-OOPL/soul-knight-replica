#include "Scene/MapTest.hpp"

#include "Component/TextButton/TextButton.hpp"
#include "Util/Time.hpp"

void MapTest::Initialize() {
    m_Player = std::make_shared<Player>();
    m_Player->SetPosition({0.0F, 0.0F});
    m_Root->AddChild(m_Player);

    m_Root->AddChild(std::make_shared<TextButton>("載入存檔", 2, nullptr));
    
    /*建立碰撞箱*/
    m_TestBlock = std::make_shared<MapPiece>(
        glm::vec2(220.0F, 0.0F), RESOURCE_DIR "/Character/character.png", true);
    m_TestBlock->m_Transform.scale = {2.0F, 2.0F};//長寬放大兩倍
    m_TestBlock->SetColliderSize(m_TestBlock->GetScaledSize());
    m_Root->AddChild(m_TestBlock);
    ////////////
    for (const auto &piece : m_Pieces) {
        m_Root->AddChild(piece);
    }
}

void MapTest::Dispose() {
    if (m_Player != nullptr) {
        m_Root->RemoveChild(m_Player);
        m_Player.reset();
    }

    if (m_TestBlock != nullptr) {
        m_Root->RemoveChild(m_TestBlock);
        m_TestBlock.reset();
    }

    for (const auto &piece : m_Pieces) {
        m_Root->RemoveChild(piece);
    }
}

void MapTest::Update() {
    const glm::vec2 moveDirection = m_Player->GetMoveIntent();
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
}

bool MapTest::WillPlayerCollide(const glm::vec2 &nextCoordinate) const {
    const ICollidable::RectCollider playerCollider{
        nextCoordinate,
        m_Player->GetColliderSize(),
    };

    for (const auto &piece : m_Pieces) {
        if (!piece->CanBlockMovement()) {
            continue;
        }

        if (ICollidable::IsOverlapping(playerCollider, piece->GetCollider())) {
            return true;
        }
    }

    if (m_TestBlock != nullptr && m_TestBlock->CanBlockMovement() &&
        ICollidable::IsOverlapping(playerCollider, m_TestBlock->GetCollider())) {
        return true;
    }

    return false;
}
