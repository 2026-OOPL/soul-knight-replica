#include <algorithm>

#include <glm/geometric.hpp>

#include "Component/BulletHitEffect.hpp"

#include "Component/Bullet.hpp"
#include "Component/Character/Character.hpp"

KnockbackHitEffect::KnockbackHitEffect(float strength)
    : m_Strength(std::max(0.0F, strength)) {
}

void KnockbackHitEffect::Apply(Character &target, const Bullet &bullet) const {
    if (this->m_Strength <= 0.0F) {
        return;
    }

    const glm::vec2 momentum = bullet.GetMomentum();
    if (glm::length(momentum) <= 0.0001F) {
        return;
    }

    target.ApplyImpulse(glm::normalize(momentum) * this->m_Strength);
}
