#include "Component/Mobs/RuinsTurret.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include <glm/geometric.hpp>

#include "Component/Bullet.hpp"
#include "Component/BulletHitEffect.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kRuinsTurretSprite = {
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_0.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_1.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_2.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_3.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_4.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_5.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_6.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_7.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_8.png",
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_walk_9.png"
};

const std::vector<std::string> kRuinsTurretDieSprite = {
    RESOURCE_DIR"/Mob/RuinsTurret/RuinsTurret_die.png"
};

const std::vector<std::string> kLaserSprite = {
    RESOURCE_DIR"/Bullet/EnemyLaserBullet.png"
};

const std::string kLaserResource = RESOURCE_DIR"/Bullet/EnemyLaserBullet.png";

constexpr float kAggroDistance = 320.0F;
constexpr float kWarningLength = 280.0F;
constexpr float kWarningDurationMs = 650.0F;
constexpr float kFiringDurationMs = 120.0F;
constexpr float kAttackCooldownMs = 900.0F;
constexpr float kWarningThicknessScale = 0.07F;
constexpr float kLaserSpeed = 1.3F;
constexpr float kLaserLifetimeMs = 1600.0F;
constexpr float kLaserKnockbackStrength = 0.12F;
constexpr int kLaserDamage = 3;

class RefractingLaserBullet : public Bullet {
public:
    RefractingLaserBullet(
        const glm::vec2 &coordinate,
        const glm::vec2 &direction,
        CombatFaction faction
    ) : Bullet(
            kLaserSprite,
            coordinate,
            direction * kLaserSpeed,
            4,
            kLaserDamage,
            faction
        ),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()) {
        this->SetColliderSize({30.0F, 8.0F});
        this->AddHitEffect(std::make_shared<KnockbackHitEffect>(kLaserKnockbackStrength));
    }

    void Update() override {
        if (Util::Time::GetElapsedTimeMs() - this->m_SpawnTime >= kLaserLifetimeMs) {
            this->RequestDestroy();
            return;
        }

        const float movementDeltaTimeMs =
            std::min(Util::Time::GetDeltaTimeMs(), 10.0F);
        const glm::vec2 frameDelta = this->m_Momentum * 0.5F * movementDeltaTimeMs;

        if (this->m_CollisionResolver == nullptr) {
            this->m_AbsoluteTransform.translation += frameDelta;
            return;
        }

        const Collision::MovementResult movementResult =
            this->m_CollisionResolver(*this, frameDelta);
        this->m_AbsoluteTransform.translation += movementResult.resolvedDelta;

        if (!movementResult.blockedX && !movementResult.blockedY) {
            return;
        }

        if (this->m_HasRefracted) {
            this->RequestDestroy();
            return;
        }

        this->m_HasRefracted = true;
        if (movementResult.blockedX) {
            this->m_Momentum.x *= -1.0F;
        }
        if (movementResult.blockedY) {
            this->m_Momentum.y *= -1.0F;
        }
        this->SetAbsoluteRotation(std::atan2(this->m_Momentum.y, this->m_Momentum.x));
    }

private:
    Util::ms_t m_SpawnTime = 0;
    bool m_HasRefracted = false;
};

} // namespace

RuinsTurretBeamVisual::RuinsTurretBeamVisual(
    const std::string &resource,
    float zIndex
) : Util::GameObject(std::make_shared<Util::Image>(resource), zIndex) {
}

Util::Transform RuinsTurretBeamVisual::GetObjectTransform() const {
    return this->m_Transform;
}

void RuinsTurretBeamVisual::ConfigureBeam(
    const glm::vec2 &start,
    const glm::vec2 &direction,
    float length,
    float thicknessScale
) {
    const glm::vec2 forward =
        glm::length(direction) <= 0.0001F ? glm::vec2(1.0F, 0.0F) : glm::normalize(direction);
    const float safeLength = std::max(1.0F, length);

    this->SetAbsoluteTranslation(start + forward * (safeLength * 0.5F));
    this->SetAbsoluteRotation(std::atan2(forward.y, forward.x));
    this->SetAbsoluteScale({safeLength / 48.0F, thicknessScale});
}

RuinsTurret::RuinsTurret(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kRuinsTurretSprite,
        kRuinsTurretSprite,
        kRuinsTurretDieSprite,
        tracePlayer,
        collisionSystem
    ) {
    this->SetMaxHealth(10);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetColliderSize({30.0F, 30.0F});
    this->m_PlayerSpeed = 0.0F;

    this->m_PositiveWarningVisual =
        std::make_shared<RuinsTurretBeamVisual>(kLaserResource, 3.8F);
    this->m_NegativeWarningVisual =
        std::make_shared<RuinsTurretBeamVisual>(kLaserResource, 3.8F);
    this->m_PositiveWarningVisual->SetVisible(false);
    this->m_NegativeWarningVisual->SetVisible(false);
}

void RuinsTurret::Update() {
    if (this->IsDead()) {
        this->SetWarningVisible(false);
        Character::Update();
        return;
    }

    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        Character::Update();
        return;
    }

    const std::shared_ptr<Character> target = this->GetTarget();
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (target == nullptr || target->IsDead()) {
        this->EnterState(TurretState::Idle);
        this->SetWarningVisible(false);
        Character::Update();
        return;
    }

    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    if (glm::length(toTarget) > 0.0001F) {
        this->m_FaceDirection = glm::normalize(toTarget);
    }

    switch (this->m_TurretState) {
    case TurretState::Idle:
        this->SetWarningVisible(false);
        if (glm::length(toTarget) <= kAggroDistance && now >= this->m_NextAttackTime) {
            this->StartWarning(target);
        }
        break;

    case TurretState::Warning:
        this->UpdateWarningVisuals();
        if (now - this->m_StateStartTime >= kWarningDurationMs) {
            this->EnterState(TurretState::Firing);
            this->m_LasersFired = false;
            this->SetWarningVisible(false);
        }
        break;

    case TurretState::Firing:
        if (!this->m_LasersFired) {
            this->FireLasers();
            this->m_LasersFired = true;
        }
        if (now - this->m_StateStartTime >= kFiringDurationMs) {
            this->EnterState(TurretState::Cooldown);
        }
        break;

    case TurretState::Cooldown:
        if (now - this->m_StateStartTime >= kAttackCooldownMs) {
            this->EnterState(TurretState::Idle);
        }
        break;
    }

    Character::Update();
}

glm::vec2 RuinsTurret::GetMoveIntent() const {
    return {0.0F, 0.0F};
}

glm::vec2 RuinsTurret::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void RuinsTurret::UpdateWeaponPresentation() {
}

std::shared_ptr<Character> RuinsTurret::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

glm::vec2 RuinsTurret::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        return this->m_FaceDirection;
    }

    return glm::normalize(direction);
}

void RuinsTurret::EnterState(TurretState state) {
    if (this->m_TurretState == state) {
        return;
    }

    this->m_TurretState = state;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();

    if (state == TurretState::Cooldown) {
        this->m_NextAttackTime = this->m_StateStartTime + kAttackCooldownMs;
    }
}

void RuinsTurret::StartWarning(const std::shared_ptr<Character> &target) {
    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    this->m_FireDirection = this->NormalizeOrFallback(toTarget);
    this->m_FaceDirection = this->m_FireDirection;
    this->m_PositiveWarningLength =
        this->ComputeRoomClippedWarningLength(this->m_FireDirection);
    this->m_NegativeWarningLength =
        this->ComputeRoomClippedWarningLength(-this->m_FireDirection);

    this->EnterState(TurretState::Warning);
    this->SetWarningVisible(true);
    this->UpdateWarningVisuals();
}

void RuinsTurret::FireLasers() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 origin = this->GetAbsoluteTranslation();
    const glm::vec2 positiveDirection = this->m_FireDirection;
    const glm::vec2 negativeDirection = -this->m_FireDirection;

    this->m_MapSystem->AddBullet(std::make_shared<RefractingLaserBullet>(
        origin + positiveDirection * 18.0F,
        positiveDirection,
        this->GetFaction()
    ));
    this->m_MapSystem->AddBullet(std::make_shared<RefractingLaserBullet>(
        origin + negativeDirection * 18.0F,
        negativeDirection,
        this->GetFaction()
    ));
}

void RuinsTurret::UpdateWarningVisuals() {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    if (this->m_PositiveWarningVisual != nullptr) {
        this->m_PositiveWarningVisual->ConfigureBeam(
            origin,
            this->m_FireDirection,
            this->m_PositiveWarningLength,
            kWarningThicknessScale
        );
    }
    if (this->m_NegativeWarningVisual != nullptr) {
        this->m_NegativeWarningVisual->ConfigureBeam(
            origin,
            -this->m_FireDirection,
            this->m_NegativeWarningLength,
            kWarningThicknessScale
        );
    }
}

float RuinsTurret::ComputeRoomClippedWarningLength(const glm::vec2 &direction) const {
    const glm::vec2 forward = this->NormalizeOrFallback(direction);
    if (this->m_MapSystem == nullptr) {
        return kWarningLength;
    }

    const std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room == nullptr) {
        return kWarningLength;
    }

    const glm::vec2 origin = this->GetAbsoluteTranslation();
    const glm::vec2 roomCenter = room->GetAbsoluteTranslation();
    const glm::vec2 roomHalfSize = room->GetAreaSize() * 0.5F;
    constexpr float kRoomEdgePadding = 8.0F;

    const float left = roomCenter.x - roomHalfSize.x + kRoomEdgePadding;
    const float right = roomCenter.x + roomHalfSize.x - kRoomEdgePadding;
    const float bottom = roomCenter.y - roomHalfSize.y + kRoomEdgePadding;
    const float top = roomCenter.y + roomHalfSize.y - kRoomEdgePadding;

    float maxLength = kWarningLength;
    if (forward.x > 0.0001F) {
        maxLength = std::min(maxLength, (right - origin.x) / forward.x);
    } else if (forward.x < -0.0001F) {
        maxLength = std::min(maxLength, (left - origin.x) / forward.x);
    }

    if (forward.y > 0.0001F) {
        maxLength = std::min(maxLength, (top - origin.y) / forward.y);
    } else if (forward.y < -0.0001F) {
        maxLength = std::min(maxLength, (bottom - origin.y) / forward.y);
    }

    return std::max(1.0F, maxLength);
}

void RuinsTurret::SetWarningVisible(bool visible) {
    if (this->m_PositiveWarningVisual != nullptr) {
        if (visible && !this->m_PositiveWarningAttached) {
            this->AddChild(this->m_PositiveWarningVisual);
            this->m_PositiveWarningAttached = true;
        } else if (!visible && this->m_PositiveWarningAttached) {
            this->RemoveChild(this->m_PositiveWarningVisual);
            this->m_PositiveWarningAttached = false;
        }
        this->m_PositiveWarningVisual->SetVisible(visible);
    }

    if (this->m_NegativeWarningVisual != nullptr) {
        if (visible && !this->m_NegativeWarningAttached) {
            this->AddChild(this->m_NegativeWarningVisual);
            this->m_NegativeWarningAttached = true;
        } else if (!visible && this->m_NegativeWarningAttached) {
            this->RemoveChild(this->m_NegativeWarningVisual);
            this->m_NegativeWarningAttached = false;
        }
        this->m_NegativeWarningVisual->SetVisible(visible);
    }
}
