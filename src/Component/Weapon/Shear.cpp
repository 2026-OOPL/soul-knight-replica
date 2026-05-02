#include "Component/Weapons/Shear.hpp"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>

#include "Util/Time.hpp"
#include "Component/BulletHitEffect.hpp"

namespace {

constexpr float kShearHitboxLifetimeMs = 120.0F;
constexpr float kShearHitboxForwardOffset = 38.0F;
constexpr float kShearKnockbackStrength = 0.16F;

glm::vec2 NormalizeOrRight(const glm::vec2 &direction) {
    if (glm::length(direction) <= 0.0001F) {
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

class ShearHitbox : public Bullet {
public:
    ShearHitbox(
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        int damage,
        CombatFaction faction
    ) : Bullet(
            std::make_shared<Util::Animation>(SHEAR_SPIKE_EFFECT, true, 40, false, 0, false),
            coordinate,
            momentum,
            1,
            damage,
            faction
        ),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()) {
        this->SetAbsoluteScale({0.0F, 0.0F});
        this->m_Transform.scale = this->m_AbsoluteTransform.scale;
        this->SetColliderSize({46.0F, 28.0F});
    }

    void Update() override {
        if (Util::Time::GetElapsedTimeMs() - this->m_SpawnTime >= kShearHitboxLifetimeMs) {
            this->RequestDestroy();
            return;
        }
    }

private:
    float m_SpawnTime;
};

} // namespace

ShearAttackEffect::ShearAttackEffect()
    : Util::GameObject(nullptr, 6) {
    this->m_Animation = std::make_shared<Util::Animation>(
        SHEAR_SPIKE_EFFECT,
        false,
        40,
        false,
        0,
        false
    );
    this->SetDrawable(this->m_Animation);
    this->SetAbsoluteScale({0.0F, 0.0F});
    this->SetVisible(false);
}

void ShearAttackEffect::Update() {
    if (Util::Time::GetElapsedTimeMs() >= this->m_EndTime) {
        this->m_Active = false;
        this->SetAbsoluteScale({0.0F, 0.0F});
        this->SetVisible(false);
    }
}

Util::Transform ShearAttackEffect::GetObjectTransform() const {
    return this->m_Transform;
}

void ShearAttackEffect::Play(const glm::vec2 &anchor, const glm::vec2 &facingDirection) {
    this->m_EndTime = Util::Time::GetElapsedTimeMs() + kShearEffectDurationMs;
    this->m_Active = true;
    this->m_Animation->SetCurrentFrame(0);
    this->m_Animation->Play();
    this->SetVisible(true);
    this->SyncToWeapon(anchor, facingDirection, 46.0F);
}

void ShearAttackEffect::SyncToWeapon(
    const glm::vec2 &anchor,
    const glm::vec2 &facingDirection,
    float distanceFromAnchor
) {
    this->m_FacingDirection = NormalizeOrRight(facingDirection);
    this->SetAbsoluteTranslation(anchor + this->m_FacingDirection * distanceFromAnchor);

    if (!this->m_Active) {
        this->SetAbsoluteScale({0.0F, 0.0F});
        return;
    }

    const float rotation = std::atan2(this->m_FacingDirection.y, this->m_FacingDirection.x);
    this->SetAbsoluteRotation(rotation);
    this->SetAbsoluteScale({1.0F, std::abs(rotation) > M_PI / 2.0F ? -1.0F : 1.0F});
}

Shear::Shear()
    : Weapon(SHEAR_WEAPON_SKIN, kShearFireDelayMs) {
    this->SetBulletDamage(kShearDamage);
    this->m_WeaponRadius = 22.0F;
    this->m_RecoilDistance = 0.0F;

    this->m_SpikeEffect = std::make_shared<ShearAttackEffect>();
    this->AddChild(this->m_SpikeEffect);
}

void Shear::Update() {
    this->SetWeaponPointingByMoveDirection();

    const glm::vec2 facingDirection = NormalizeOrRight(this->m_FacingDirection);
    const float rotation = std::atan2(facingDirection.y, facingDirection.x);
    this->SetAbsoluteRotation(rotation);
    this->SetAbsoluteScale({1.0F, std::abs(rotation) > M_PI / 2.0F ? -1.0F : 1.0F});
    this->SetAbsoluteTranslation(
        this->m_AnchorPoint +
        facingDirection * (this->m_WeaponRadius + this->GetThrustOffset())
    );

    this->SyncSpikeEffect();
}

bool Shear::ShotBullet() {
    const bool fired = Weapon::ShotBullet();
    if (fired) {
        this->TriggerThrust();
    }

    return fired;
}

std::shared_ptr<Bullet> Shear::CreateBullet() const {
    const glm::vec2 facingDirection = NormalizeOrRight(this->m_FacingDirection);
    std::shared_ptr<Bullet> hitbox = std::make_shared<ShearHitbox>(
        this->m_AnchorPoint + facingDirection * kShearHitboxForwardOffset,
        facingDirection,
        this->GetBulletDamage(),
        this->GetProjectileFaction()
    );

    hitbox->AddHitEffect(std::make_shared<KnockbackHitEffect>(kShearKnockbackStrength));
    return hitbox;
}

void Shear::TriggerThrust() {
    this->m_ThrustStartTime = Util::Time::GetElapsedTimeMs();
    this->m_ThrustEndTime = this->m_ThrustStartTime + kShearThrustDurationMs;

    if (this->m_SpikeEffect != nullptr) {
        this->m_SpikeEffect->Play(this->m_AnchorPoint, this->m_FacingDirection);
    }
}

float Shear::GetThrustOffset() const {
    const float now = Util::Time::GetElapsedTimeMs();
    if (now >= this->m_ThrustEndTime || this->m_ThrustEndTime <= this->m_ThrustStartTime) {
        return 0.0F;
    }

    const float progress =
        (now - this->m_ThrustStartTime) / (this->m_ThrustEndTime - this->m_ThrustStartTime);
    const float triangleWave = progress <= 0.5F ? progress * 2.0F : (1.0F - progress) * 2.0F;
    return kShearThrustDistance * std::clamp(triangleWave, 0.0F, 1.0F);
}

void Shear::SyncSpikeEffect() {
    if (this->m_SpikeEffect == nullptr) {
        return;
    }

    this->m_SpikeEffect->SyncToWeapon(
        this->m_AnchorPoint,
        this->m_FacingDirection,
        this->m_WeaponRadius + this->GetThrustOffset() + 22.0F
    );
}
