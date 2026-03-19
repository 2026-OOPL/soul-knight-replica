#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"

#include <utility>

#include "Component/Player/Player.hpp"

glm::vec2 Player::GetObjectSize() {
    return this->GetScaledSize();
}

glm::vec2 Player::GetCooridinate() {
    return this->m_Cooridinate;
}

Util::Transform Player::GetTransform() {
    return this->m_Transform;
}

std::vector<std::shared_ptr<Collider>> Player::GetCollideBox() {
    // Legacy interface, the current collision flow uses CollisionSystem directly.
    return {};
}

glm::vec2 Player::GetColliderSize() {
    return m_ColliderSize;
}

void Player::SetColliderSize(const glm::vec2 &colliderSize) {
    m_ColliderSize = colliderSize;
}

glm::vec2 Player::GetPosition() const {
    return m_Cooridinate;
}

void Player::SetPosition(const glm::vec2 &position) {
    m_Cooridinate = position;
}

Collision::AxisAlignedBox Player::GetCollisionBox() const {
    return this->GetCollisionBoxAt(m_Cooridinate);
}

Collision::AxisAlignedBox Player::GetCollisionBoxAt(const glm::vec2 &coordinate) const {
    return Collision::CollisionSystem::BuildBox(coordinate, m_ColliderSize);
}

void Player::SetCollisionResolver(CollisionResolver collisionResolver) {
    m_CollisionResolver = std::move(collisionResolver);
}

glm::vec2 Player::GetMoveIntent() const {
    glm::vec2 moveIntent(0.0F, 0.0F);

    if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
        moveIntent.y += 1.0F;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::S)) {
        moveIntent.y -= 1.0F;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        moveIntent.x -= 1.0F;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        moveIntent.x += 1.0F;
    }

    if (moveIntent == glm::vec2(0.0F, 0.0F)) {
        return moveIntent;
    }
    
    /*為了移除斜線走路比直線快的bug*/
    return glm::normalize(moveIntent);
}

void Player::Update() {
    const glm::vec2 moveDirection = this->GetMoveIntent();

    if (moveDirection == glm::vec2(0.0F, 0.0F)) {
        m_PendingMoveDelta = {0.0F, 0.0F};
        return;
    }

    const glm::vec2 frameDelta =
        moveDirection * this->m_PlayerSpeed * Util::Time::GetDeltaTimeMs();

    m_PendingMoveDelta = frameDelta;

    if (m_CollisionResolver) {
        const Collision::MovementResult movementResult = m_CollisionResolver(
            this->GetCollisionBox(),
            frameDelta
        );

        this->m_Cooridinate += movementResult.resolvedDelta;
    } else {
        this->m_Cooridinate += frameDelta;
    }
}

bool Player::WillCollide() {
    if (!m_CollisionResolver) {
        return false;
    }

    const Collision::MovementResult movementResult = m_CollisionResolver(
        this->GetCollisionBox(),
        m_PendingMoveDelta
    );

    return movementResult.blockedX || movementResult.blockedY;
}
