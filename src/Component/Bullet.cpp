#include <glm/vec2.hpp>

#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

#include "Component/Bullet.hpp"

Bullet::Bullet(
    std::shared_ptr<Util::Animation> animation,
    glm::vec2 cooridinate,
    glm::vec2 momentum,
    int zIndex
) : Util::GameObject(nullptr, zIndex) {
    this->m_Animation = animation;
    this->m_Momentum = momentum;

    this->m_AbsoluteTransform.translation = cooridinate;

    this->SetDrawable(this->m_Animation);

    // Set initial pointing direction
    float rotation = atan2(this->m_Momentum.y, this->m_Momentum.x);

    if (fabs(rotation) > M_PI / 2.0f) {
        this->SetAbsoluteScale(
            this->GetAbsoluteScale() * glm::vec2(1, -1)
        );
    }

    this->SetAbsoluteRotation(rotation);
}

Bullet::Bullet(
    const std::vector<std::string>& sprite,
    glm::vec2 cooridinate,
    glm::vec2 momentum,
    int zIndex
) : Bullet(
    std::make_shared<Util::Animation>(sprite, 20, true),
    cooridinate,
    momentum,
    zIndex
) {}

Util::Transform Bullet::GetObjectTransform() const {
    return this->m_Transform;
}

void Bullet::Update() {
    const float movementDeltaTimeMs =
        std::min(Util::Time::GetDeltaTimeMs(), 10.0F);

    const glm::vec2 frameDelta =
        m_Momentum * 0.5F * movementDeltaTimeMs;

    this->m_AbsoluteTransform.translation += frameDelta;
}