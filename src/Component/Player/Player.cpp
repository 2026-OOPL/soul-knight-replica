#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

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
    // TODO
    return {};
}

glm::vec2 Player::GetColliderSize() {
    return m_ColliderSize;
}

void Player::SetColliderSize(const glm::vec2 &colliderSize) {
    m_ColliderSize = colliderSize;
}

glm::vec2 Player::GetPosition() const {
    return m_Transform.translation;
}

void Player::SetPosition(const glm::vec2 &position) {
    m_Transform.translation = position;
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