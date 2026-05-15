#include "Component/Mobs/RuinsGuard.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include <glm/geometric.hpp>

#include "Common/CombatFaction.hpp"
#include "Component/Bullet.hpp"
#include "Component/BulletHitEffect.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kEnemyRoundBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyRoundBullet.png"
};

const std::vector<std::string> kChargeEffectSprite = {
    RESOURCE_DIR"/Effect/Charge/Charge_0.png",
    RESOURCE_DIR"/Effect/Charge/Charge_1.png",
    RESOURCE_DIR"/Effect/Charge/Charge_2.png",
    RESOURCE_DIR"/Effect/Charge/Charge_3.png"
};

constexpr float kIdleDistance = 320.0F;
constexpr float kPikeDamage = 3;
constexpr float kPikeRange = 46.0F;
constexpr float kPikeDurationMs = 260.0F;
constexpr float kPikeHitboxTimeMs = 120.0F;
constexpr float kDefenseDurationMs = 1600.0F;
constexpr float kChargeWindupMs = 360.0F;
constexpr float kChargeRecoverMs = 420.0F;
constexpr float kChargeSpeed = 0.34F;
constexpr float kChargeDamage = 5.0F;
constexpr float kChargeKnockbackStrength = 0.18F;
constexpr float kChargeContactCooldownMs = 450.0F;
constexpr int kMaxChargeBounces = 3;
constexpr float kRadialShotDurationMs = 420.0F;
constexpr int kRadialBulletCount = 18;
constexpr float kRadialBulletSpeed = 0.32F;
constexpr float kRadialBulletLifetimeMs = 2200.0F;
constexpr float kBulletKnockbackStrength = 0.08F;
constexpr float kPi = 3.14159265358979323846F;

glm::vec2 NormalizeOrRight(const glm::vec2 &direction) {
    if (glm::length(direction) <= 0.0001F) {
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

class TimedBullet : public Bullet {
public:
    TimedBullet(
        const std::vector<std::string> &sprite,
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    ) : Bullet(sprite, coordinate, momentum, 1, damage, faction),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()),
        m_LifetimeMs(lifetimeMs) {
    }

    void Update() override {
        if (Util::Time::GetElapsedTimeMs() - this->m_SpawnTime >= this->m_LifetimeMs) {
            this->RequestDestroy();
            return;
        }

        Bullet::Update();
    }

private:
    float m_SpawnTime = 0.0F;
    float m_LifetimeMs = 0.0F;
};

class PikeHitbox : public Bullet {
public:
    PikeHitbox(
        const glm::vec2 &coordinate,
        const glm::vec2 &direction,
        CombatFaction faction
    ) : Bullet(
            std::shared_ptr<Util::Animation>(),
            coordinate,
            direction,
            1,
            static_cast<int>(kPikeDamage),
            faction
        ),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()) {
        this->SetVisible(false);
        this->SetColliderSize({46.0F, 28.0F});
    }

    void Update() override {
        if (Util::Time::GetElapsedTimeMs() - this->m_SpawnTime >= kPikeHitboxTimeMs) {
            this->RequestDestroy();
        }
    }

private:
    float m_SpawnTime = 0.0F;
};

std::shared_ptr<RuinsGuardEquipmentVisual> BuildEquipmentVisual(
    const std::string &resource,
    float zIndex
) {
    return std::make_shared<RuinsGuardEquipmentVisual>(
        std::make_shared<Util::Image>(resource),
        zIndex
    );
}

} // namespace

RuinsGuard::RuinsGuard(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        RUINS_GUARD_STAND_SPRITE,
        RUINS_GUARD_WALK_SPRITE,
        RUINS_GUARD_DIE_SPRITE,
        tracePlayer,
        collisionSystem
    ) {
    this->SetMaxHealth(12);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetWeapon(nullptr);

    this->m_PikeVisual = BuildEquipmentVisual(RESOURCE_DIR"/Weapon/Pike.png", 5.0F);
    this->m_ShieldVisual = BuildEquipmentVisual(RESOURCE_DIR"/Weapon/Shield.png", 5.0F);
    this->m_ChargeEffectAnimation = std::make_shared<Util::Animation>(
        kChargeEffectSprite,
        true,
        35,
        true,
        0,
        false
    );
    this->m_ChargeEffectVisual =
        std::make_shared<RuinsGuardEquipmentVisual>(this->m_ChargeEffectAnimation, 6.2F);
    this->m_ChargeEffectVisual->SetVisible(false);
    this->AddChild(this->m_PikeVisual);
    this->AddChild(this->m_ShieldVisual);
    this->EnterState(GuardState::Idle);
}

void RuinsGuard::Update() {
    if (this->IsDead()) {
        if (this->m_PikeVisual != nullptr) {
            this->RemoveChild(this->m_PikeVisual);
            this->m_PikeVisual = nullptr;
        }
        if (this->m_ShieldVisual != nullptr) {
            this->RemoveChild(this->m_ShieldVisual);
            this->m_ShieldVisual = nullptr;
        }
        if (this->m_ChargeEffectVisual != nullptr) {
            this->SetChargeEffectActive(false);
            this->m_ChargeEffectVisual = nullptr;
        }
        Character::Update();
        return;
    }

    this->UpdateState();
    Character::Update();
    this->UpdateEquipmentPresentation();
}

glm::vec2 RuinsGuard::GetMoveIntent() const {
    return this->m_MoveIntent;
}

glm::vec2 RuinsGuard::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void RuinsGuard::ApplyDamage(int damage) {
    if (this->m_GuardState == GuardState::Defense) {
        return;
    }

    Mob::ApplyDamage(damage);
}

void RuinsGuard::UpdateWeaponPresentation() {
}

std::shared_ptr<Character> RuinsGuard::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

glm::vec2 RuinsGuard::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        return NormalizeOrRight(this->m_FaceDirection);
    }

    return glm::normalize(direction);
}

void RuinsGuard::UpdateState() {
    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        return;
    }

    const std::shared_ptr<Character> target = this->GetTarget();
    if (target == nullptr || target->IsDead()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        this->EnterState(GuardState::Idle);
        return;
    }

    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    const float distanceToTarget = glm::length(toTarget);
    if (distanceToTarget > 0.0001F) {
        this->m_FaceDirection = glm::normalize(toTarget);
    }

    switch (this->m_GuardState) {
    case GuardState::Idle:
    case GuardState::Pursuit:
        this->TryStartAttackByDistance(distanceToTarget);
        break;

    case GuardState::PikeAttack:
        this->UpdatePikeAttack();
        break;

    case GuardState::Defense:
        this->UpdateDefense();
        break;

    case GuardState::ChargeWindup:
        this->UpdateChargeWindup();
        break;

    case GuardState::Charging:
        this->UpdateCharging();
        break;

    case GuardState::ChargeRecover:
        this->UpdateChargeRecover();
        break;

    case GuardState::RadialShot:
        if (!this->m_RadialShotFired) {
            this->FireRadialBullets();
            this->m_RadialShotFired = true;
        }
        if (Util::Time::GetElapsedTimeMs() - this->m_StateStartTime >= kRadialShotDurationMs) {
            this->StartDefense();
        }
        break;
    }
}

void RuinsGuard::TryStartAttackByDistance(float distanceToTarget) {
    if (distanceToTarget > kIdleDistance) {
        this->m_MoveIntent = {0.0F, 0.0F};
        this->EnterState(GuardState::Idle);
        return;
    }

    this->m_NextCycleAttack = CycleAttack::Charge;
    this->StartDefense();
}

void RuinsGuard::EnterState(GuardState state) {
    if (this->m_GuardState == state) {
        return;
    }

    this->m_GuardState = state;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
}

void RuinsGuard::StartPikeAttack(const glm::vec2 &targetPosition) {
    this->EnterState(GuardState::PikeAttack);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_PikeAttackDirection =
        this->NormalizeOrFallback(targetPosition - this->GetAbsoluteTranslation());
    this->m_FaceDirection = this->m_PikeAttackDirection;
    this->m_PikeHitboxFired = false;
}

void RuinsGuard::StartDefense() {
    this->EnterState(GuardState::Defense);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_NextDecisionTime =
        Util::Time::GetElapsedTimeMs() + kDefenseDurationMs + 300.0F;
}

void RuinsGuard::StartChargeWindup(const glm::vec2 &targetPosition) {
    this->EnterState(GuardState::ChargeWindup);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_ChargeVelocity =
        this->NormalizeOrFallback(targetPosition - this->GetAbsoluteTranslation()) *
        kChargeSpeed;
    this->m_FaceDirection = this->NormalizeOrFallback(this->m_ChargeVelocity);
    this->m_ChargeBounceCount = 0;
}

void RuinsGuard::StartRadialShot() {
    this->EnterState(GuardState::RadialShot);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_RadialShotFired = false;
}

void RuinsGuard::UpdatePikeAttack() {
    const float elapsed = Util::Time::GetElapsedTimeMs() - this->m_StateStartTime;
    if (!this->m_PikeHitboxFired && elapsed >= 90.0F) {
        this->FirePikeHitbox();
        this->m_PikeHitboxFired = true;
    }

    if (elapsed >= kPikeDurationMs) {
        this->StartDefense();
    }
}

void RuinsGuard::UpdateDefense() {
    if (Util::Time::GetElapsedTimeMs() - this->m_StateStartTime >= kDefenseDurationMs) {
        const std::shared_ptr<Character> target = this->GetTarget();
        if (target == nullptr ||
            glm::distance(target->GetAbsoluteTranslation(), this->GetAbsoluteTranslation()) >
                kIdleDistance) {
            this->EnterState(GuardState::Idle);
            return;
        }

        if (this->m_NextCycleAttack == CycleAttack::Charge) {
            this->m_NextCycleAttack = CycleAttack::RadialShot;
            this->StartChargeWindup(target->GetAbsoluteTranslation());
            return;
        }

        if (this->m_NextCycleAttack == CycleAttack::RadialShot) {
            this->m_NextCycleAttack = CycleAttack::Charge;
            this->StartRadialShot();
            return;
        }
    }
}

void RuinsGuard::UpdateChargeWindup() {
    if (Util::Time::GetElapsedTimeMs() - this->m_StateStartTime >= kChargeWindupMs) {
        this->EnterState(GuardState::Charging);
    }
}

void RuinsGuard::UpdateCharging() {
    const float deltaTimeMs =
        std::min(Util::Time::GetDeltaTimeMs(), 50.0F);
    const glm::vec2 intendedDelta = this->m_ChargeVelocity * deltaTimeMs;

    if (this->m_CollisionResolver != nullptr) {
        const Collision::MovementResult result =
            this->m_CollisionResolver(*this, intendedDelta);
        this->m_AbsoluteTransform.translation += result.resolvedDelta;

        if (result.blockedX) {
            this->m_ChargeVelocity.x *= -1.0F;
        }
        if (result.blockedY) {
            this->m_ChargeVelocity.y *= -1.0F;
        }
        if (result.blockedX || result.blockedY) {
            this->m_ChargeBounceCount++;
            this->m_FaceDirection = this->NormalizeOrFallback(this->m_ChargeVelocity);
        }
    } else {
        this->m_AbsoluteTransform.translation += intendedDelta;
    }

    this->DamagePlayerOnChargeContact();

    if (this->m_ChargeBounceCount >= kMaxChargeBounces) {
        this->EnterState(GuardState::ChargeRecover);
        this->m_MoveIntent = {0.0F, 0.0F};
    }
}

void RuinsGuard::UpdateChargeRecover() {
    if (Util::Time::GetElapsedTimeMs() - this->m_StateStartTime >= kChargeRecoverMs) {
        this->m_NextCycleAttack = CycleAttack::Charge;
        this->StartRadialShot();
    }
}

void RuinsGuard::FirePikeHitbox() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    std::shared_ptr<Bullet> hitbox = std::make_shared<PikeHitbox>(
        this->GetAbsoluteTranslation() + this->m_PikeAttackDirection * kPikeRange,
        this->m_PikeAttackDirection,
        this->GetFaction()
    );
    hitbox->AddHitEffect(std::make_shared<KnockbackHitEffect>(0.12F));
    this->m_MapSystem->AddBullet(hitbox);
}

void RuinsGuard::FireRadialBullets() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    for (int index = 0; index < kRadialBulletCount; ++index) {
        const float angle =
            2.0F * kPi * static_cast<float>(index) /
            static_cast<float>(kRadialBulletCount);
        const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
        std::shared_ptr<Bullet> bullet = std::make_shared<TimedBullet>(
            kEnemyRoundBulletSprite,
            this->GetAbsoluteTranslation() + direction * 16.0F,
            direction * kRadialBulletSpeed,
            3,
            this->GetFaction(),
            kRadialBulletLifetimeMs
        );
        bullet->SetColliderSize({12.0F, 12.0F});
        bullet->SetAbsoluteScale({0.6F, 0.6F});
        bullet->AddHitEffect(std::make_shared<KnockbackHitEffect>(kBulletKnockbackStrength));
        this->m_MapSystem->AddBullet(bullet);
    }
}

void RuinsGuard::DamagePlayerOnChargeContact() {
    if (this->m_MapSystem == nullptr ||
        Util::Time::GetElapsedTimeMs() - this->m_LastContactDamageTime <
            kChargeContactCooldownMs) {
        return;
    }

    Collision::CollisionSystem collisionSystem;
    const Collision::AxisAlignedBox ownBox = this->GetCollisionBox();
    for (const auto &player : this->m_MapSystem->GetPlayers()) {
        if (player == nullptr || player->IsDead()) {
            continue;
        }

        if (!collisionSystem.IsOverlapping(ownBox, player->GetCollisionBox())) {
            continue;
        }

        player->ApplyDamage(static_cast<int>(kChargeDamage));
        this->KnockbackPlayerFromGuard(*player, kChargeKnockbackStrength);
        this->m_LastContactDamageTime = Util::Time::GetElapsedTimeMs();
    }
}

void RuinsGuard::KnockbackPlayerFromGuard(Player &player, float strength) const {
    player.ApplyImpulse(
        this->NormalizeOrFallback(
            player.GetAbsoluteTranslation() - this->GetAbsoluteTranslation()
        ) * strength
    );
}

void RuinsGuard::UpdateEquipmentPresentation() {
    switch (this->m_GuardState) {
    case GuardState::Defense:
        this->UpdateDefenseEquipmentPresentation();
        break;

    case GuardState::ChargeWindup:
    case GuardState::Charging:
    case GuardState::ChargeRecover:
        this->UpdateChargeEquipmentPresentation();
        break;

    case GuardState::Idle:
    case GuardState::Pursuit:
    case GuardState::RadialShot:
        this->UpdateIdleEquipmentPresentation();
        break;

    case GuardState::PikeAttack:
        this->UpdateCombatEquipmentPresentation();
        break;
    }
}

void RuinsGuard::UpdateIdleEquipmentPresentation() {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    this->SetChargeEffectActive(false);

    if (this->m_ShieldVisual != nullptr) {
        this->m_ShieldVisual->SetZIndex(5.5F);
        this->m_ShieldVisual->SetVisible(true);
        this->m_ShieldVisual->SetAbsoluteTranslation(origin + glm::vec2(-11.0F, -11.0F));
        this->m_ShieldVisual->SetAbsoluteRotation(0.0F);
        this->m_ShieldVisual->SetAbsoluteScale({1.0F, 1.0F});
    }

    if (this->m_PikeVisual != nullptr) {
        this->m_PikeVisual->SetZIndex(5.5F);
        this->m_PikeVisual->SetVisible(true);
        this->m_PikeVisual->SetAbsoluteTranslation(origin + glm::vec2(17.0F, -2.0F));
        this->m_PikeVisual->SetAbsoluteRotation(kPi / 2.0F);
        this->m_PikeVisual->SetAbsoluteScale({1.0F, 1.0F});
    }
}

void RuinsGuard::UpdateCombatEquipmentPresentation() {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    const glm::vec2 direction = this->NormalizeOrFallback(this->m_PikeAttackDirection);
    const float rotation = std::atan2(direction.y, direction.x);
    this->SetChargeEffectActive(false);

    if (this->m_ShieldVisual != nullptr) {
        this->m_ShieldVisual->SetZIndex(3.5F);
        this->m_ShieldVisual->SetAbsoluteTranslation(origin + glm::vec2(-8.0F, -9.0F));
        this->m_ShieldVisual->SetAbsoluteRotation(0.0F);
    }

    if (this->m_PikeVisual != nullptr) {
        this->m_PikeVisual->SetZIndex(5.8F);
        this->m_PikeVisual->SetAbsoluteTranslation(origin + direction * 26.0F);
        this->m_PikeVisual->SetAbsoluteRotation(rotation);
        this->m_PikeVisual->SetAbsoluteScale({1.0F, std::abs(rotation) > kPi / 2.0F ? -1.0F : 1.0F});
    }
}

void RuinsGuard::UpdateDefenseEquipmentPresentation() {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    const glm::vec2 direction = this->NormalizeOrFallback(this->m_FaceDirection);
    const glm::vec2 side = {-direction.y, direction.x};
    this->SetChargeEffectActive(false);

    if (this->m_ShieldVisual != nullptr) {
        this->m_ShieldVisual->SetZIndex(6.0F);
        this->m_ShieldVisual->SetAbsoluteTranslation(origin);
        this->m_ShieldVisual->SetAbsoluteRotation(std::atan2(direction.y, direction.x));
    }

    if (this->m_PikeVisual != nullptr) {
        this->m_PikeVisual->SetZIndex(3.5F);
        this->m_PikeVisual->SetAbsoluteTranslation(origin - direction * 10.0F + side * 12.0F);
        this->m_PikeVisual->SetAbsoluteRotation(kPi / 2.0F);
    }
}

void RuinsGuard::UpdateChargeEquipmentPresentation() {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    const glm::vec2 direction = this->NormalizeOrFallback(this->m_FaceDirection);
    const float rotation = std::atan2(direction.y, direction.x);
    const bool showChargeEffect =
        this->m_GuardState == GuardState::ChargeWindup ||
        this->m_GuardState == GuardState::Charging;
    this->SetChargeEffectActive(showChargeEffect);

    if (this->m_ChargeEffectVisual != nullptr) {
        this->m_ChargeEffectVisual->SetZIndex(6.2F);
        this->m_ChargeEffectVisual->SetAbsoluteTranslation(origin + direction * 30.0F);
        this->m_ChargeEffectVisual->SetAbsoluteRotation(rotation);
        this->m_ChargeEffectVisual->SetAbsoluteScale({
            1.0F,
            std::abs(rotation) > kPi / 2.0F ? -1.0F : 1.0F
        });
    }

    if (this->m_ShieldVisual != nullptr) {
        this->m_ShieldVisual->SetZIndex(3.2F);
        this->m_ShieldVisual->SetAbsoluteTranslation(origin - direction * 12.0F);
        this->m_ShieldVisual->SetAbsoluteRotation(rotation);
    }

    if (this->m_PikeVisual != nullptr) {
        this->m_PikeVisual->SetZIndex(5.8F);
        this->m_PikeVisual->SetAbsoluteTranslation(origin + direction * 20.0F);
        this->m_PikeVisual->SetAbsoluteRotation(rotation);
    }
}

void RuinsGuard::SetChargeEffectActive(bool active) {
    if (this->m_ChargeEffectVisual == nullptr) {
        return;
    }

    if (active && !this->m_ChargeEffectAttached) {
        this->AddChild(this->m_ChargeEffectVisual);
        this->m_ChargeEffectAttached = true;
    } else if (!active && this->m_ChargeEffectAttached) {
        this->RemoveChild(this->m_ChargeEffectVisual);
        this->m_ChargeEffectAttached = false;
    }

    this->m_ChargeEffectVisual->SetVisible(active);
}
