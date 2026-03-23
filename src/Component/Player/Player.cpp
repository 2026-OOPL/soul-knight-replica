#include "Component/Player/Player.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"

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
    return {};
}

glm::vec2 Player::GetColliderSize() {
    return this->m_ColliderSize;
}

void Player::SetColliderSize(const glm::vec2 &colliderSize) {
    this->m_ColliderSize = colliderSize;
}

glm::vec2 Player::GetPosition() const {
    return this->m_Cooridinate;
}

void Player::SetPosition(const glm::vec2 &position) {
    this->m_Cooridinate = position;
    this->m_Transform.translation = position;
}

void Player::SetCollisionResolver(CollisionResolver collisionResolver) {
    this->m_CollisionResolver = collisionResolver;
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

    return glm::normalize(moveIntent);
}

void Player::Update() {
    const glm::vec2 moveDirection = this->GetMoveIntent();

    if (moveDirection != glm::vec2(0.0F, 0.0F)) {
        const glm::vec2 frameDelta =
            moveDirection * this->m_PlayerSpeed * Util::Time::GetDeltaTimeMs();

        if (this->m_CollisionResolver) {
            const Collision::MovementResult movementResult = this->m_CollisionResolver(
                Collision::CollisionSystem::BuildBox(
                    this->m_Cooridinate,
                    this->m_ColliderSize
                ),
                frameDelta
            );

            this->m_Cooridinate += movementResult.resolvedDelta;
        } else {
            this->m_Cooridinate += frameDelta;
        }
    }

    this->m_Transform.translation = this->m_Cooridinate;
}

bool Player::WillCollide() {
    return false;
}
