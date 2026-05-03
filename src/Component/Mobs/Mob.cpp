#include <algorithm>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include "Component/Mobs/Mob.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Character/Character.hpp"
#include "Util/Logger.hpp"

namespace {

constexpr float kMeleeAttackDebugDurationMs = 120.0F;

}

void Mob::Update() {
    if (this->IsDead()) {
        Character::Update();
        return;
    }

    m_AI->Update();
    Character::Update();

    if (m_AI->GetAttackTrigger()) {
        bool result = false;
        if (m_Weapon != nullptr) {
            result = m_Weapon->ShotBullet();
        } else {
            result = this->TryMeleeAttack();
        }

        if (result) {
            this->TriggerAttackVisual();
            LOG_INFO(result);
        }
    }
}

void Mob::ApplyDamage(int damage) {
    if (!this->m_DamageEnabled) {
        return;
    }

    Character::ApplyDamage(damage);
}

void Mob::SetDamageEnabled(bool enabled) {
    this->m_DamageEnabled = enabled;
}

bool Mob::IsDamageEnabled() const {
    return this->m_DamageEnabled;
}

bool Mob::GetMeleeAttackDebugBox(Collision::AxisAlignedBox &box) const {
    if (Util::Time::GetElapsedTimeMs() >= this->m_MeleeAttackDebugEndTime) {
        return false;
    }

    box = this->m_MeleeAttackDebugBox;
    return true;
}

void Mob::SetMeleeAttack(
    int damage,
    float range,
    float cooldownMs,
    float knockbackStrength
) {
    this->m_MeleeDamage = std::max(0, damage);
    this->m_MeleeRange = std::max(0.0F, range);
    this->m_MeleeCooldownMs = std::max(0.0F, cooldownMs);
    this->m_MeleeKnockbackStrength = std::max(0.0F, knockbackStrength);
}

bool Mob::TryMeleeAttack() {
    if (this->m_MeleeDamage <= 0 || this->m_MeleeRange <= 0.0F) {
        return false;
    }

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (now - this->m_LastMeleeAttackTime < this->m_MeleeCooldownMs) {
        return false;
    }

    std::shared_ptr<Character> target = this->m_TracePlayerTemp.lock();
    if (target == nullptr || target->IsDead()) {
        return false;
    }

    const float selfRadius =
        std::max(this->GetColliderSize().x, this->GetColliderSize().y) * 0.5F;
    const float targetRadius =
        std::max(target->GetColliderSize().x, target->GetColliderSize().y) * 0.5F;
    const float attackDistance =
        this->m_MeleeRange + selfRadius + targetRadius;

    if (glm::distance(this->GetAbsoluteTranslation(), target->GetAbsoluteTranslation()) >
        attackDistance) {
        return false;
    }

    this->m_LastMeleeAttackTime = now;
    target->ApplyDamage(this->m_MeleeDamage);

    const float meleeDebugExtent = (this->m_MeleeRange + selfRadius) * 2.0F;
    this->m_MeleeAttackDebugBox.center = this->GetAbsoluteTranslation();
    this->m_MeleeAttackDebugBox.size = {meleeDebugExtent, meleeDebugExtent};
    this->m_MeleeAttackDebugEndTime = now + kMeleeAttackDebugDurationMs;

    if (this->m_MeleeKnockbackStrength > 0.0F) {
        glm::vec2 knockbackDirection =
            target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
        if (glm::length(knockbackDirection) <= 0.0001F) {
            knockbackDirection = this->GetFaceDirection();
        }
        if (glm::length(knockbackDirection) > 0.0001F) {
            target->ApplyImpulse(
                glm::normalize(knockbackDirection) * this->m_MeleeKnockbackStrength
            );
        }
    }

    return true;
}

void Mob::UpdateWeaponPresentation() {
    if (this->m_Weapon == nullptr) {
        return;
    }

    this->m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
    this->m_Weapon->SetSocketOffset(this->m_WeaponSocketOffset);
    this->m_Weapon->SetFacingDirection(this->GetFaceDirection());
}

glm::vec2 Mob::GetMoveIntent() const {
    return m_AI->GetMoveDirection();
}

glm::vec2 Mob::GetFaceDirection() const {
    return m_AI->GetFaceDirection();
}

void Mob::Initialize(MapSystem* mapSystem) {
    m_MapSystem = mapSystem;

    if (m_Weapon != nullptr) {
        m_Weapon->SetOnBulletFired(
            [mapSystem](std::shared_ptr<Bullet> bullet) {
                mapSystem->AddBullet(bullet);
            }
        );

    }
}
