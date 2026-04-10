#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include "Component/Player/Player.hpp"

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

    Collision::CollisionFilter filter = this->GetCollisionFilter();
    filter.layer = Collision::CollisionLayer::Player;
    filter.mask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop) |
        Collision::ToMask(Collision::CollisionLayer::Enemy) |
        Collision::ToMask(Collision::CollisionLayer::EnemyProjectile) |
        Collision::ToMask(Collision::CollisionLayer::Trigger);
    filter.blocking = true;
    this->SetCollisionFilter(filter);
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
