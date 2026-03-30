#include <algorithm>
#include <memory>
#include <utility>

#include "Component/Bullet.hpp"
#include "Component/Bullets/TestBullet.hpp"
#include "Component/Character/Character.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"

#include "Component/Player/Player.hpp"

namespace {

constexpr float kMaxPlayerMovementDeltaTimeMs = 50.0F; // 調整玩家移動使用的最大 dt，避免卡頓後瞬移。

} // namespace

Player::Player(
    const std::vector<std::string>& StandSprite,
    const std::vector<std::string>& WalkSprite,
    const std::vector<std::string>& DieSprite
) : Character(
    StandSprite,
    WalkSprite,
    DieSprite,
    4
) {
    this->m_AbsoluteTransform.translation = {0.0F, 0.0F};
}

glm::vec2 Player::GetColliderSize() {
    return this->m_ColliderSize;
}

void Player::SetColliderSize(const glm::vec2 &colliderSize) {
    this->m_ColliderSize = colliderSize;
}

Collision::AxisAlignedBox Player::GetCollisionBox() const {
    return this->GetCollisionBoxAt(this->m_AbsoluteTransform.translation);
}

Collision::AxisAlignedBox Player::GetCollisionBoxAt(const glm::vec2 &coordinate) const {
    return Collision::CollisionSystem::BuildBox(coordinate, this->m_ColliderSize);
}

void Player::SetCollisionResolver(CollisionResolver collisionResolver) {
    this->m_CollisionResolver = std::move(collisionResolver);
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
    Character::Update();

    const glm::vec2 moveDirection = this->GetMoveIntent();

    if (moveDirection == glm::vec2(0.0F, 0.0F)) {
        this->m_PendingMoveDelta = {0.0F, 0.0F};
    } else {
        const float movementDeltaTimeMs =
            std::min(Util::Time::GetDeltaTimeMs(), kMaxPlayerMovementDeltaTimeMs);
        const glm::vec2 frameDelta =
            moveDirection * this->m_PlayerSpeed * movementDeltaTimeMs;

        this->m_PendingMoveDelta = frameDelta;

        if (this->m_CollisionResolver) {
            const Collision::MovementResult movementResult = this->m_CollisionResolver(
                this->GetCollisionBox(),
                frameDelta
            );

            this->m_AbsoluteTransform.translation += movementResult.resolvedDelta;
        } else {
            this->m_AbsoluteTransform.translation += frameDelta;
        }
    }
}
