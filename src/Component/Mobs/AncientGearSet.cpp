#include "Component/Mobs/AncientGearSet.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include <glm/geometric.hpp>

#include "Common/Constants.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kAncientGearSetStandSprite = {
    RESOURCE_DIR"/Mob/AncientGearSet/AncientGearSet_stand.png"
};

const std::vector<std::string> kAncientGearSetDieSprite = {
    RESOURCE_DIR"/Mob/AncientGearSet/AncientGearSet_die.png"
};

const std::string kLaserSprite = RESOURCE_DIR"/Bullet/48px-laser-enemy_processed.png";

constexpr float kAggroDistance = 300.0F;
constexpr float kNormalLaserMaxLength = MAP_PIXEL_PER_BLOCK * 5.5F;
constexpr float kWarningDurationMs = 650.0F;
constexpr float kLaserDurationMs = 120.0F;
constexpr float kAttackCooldownMs = 900.0F;
constexpr float kWarningThicknessScale = 0.07F;
constexpr float kLaserThicknessScale = 0.35F;
constexpr float kLaserHitHalfThickness = 8.0F;
constexpr float kLaserKnockbackStrength = 0.14F;
constexpr int kLaserDamage = 2;
constexpr float kPi = 3.14159265358979323846F;

float DistancePointToSegment(
    const glm::vec2 &point,
    const glm::vec2 &start,
    const glm::vec2 &end
) {
    const glm::vec2 segment = end - start;
    const float lengthSquared = glm::dot(segment, segment);
    if (lengthSquared <= 0.0001F) {
        return glm::distance(point, start);
    }

    const float t = std::clamp(
        glm::dot(point - start, segment) / lengthSquared,
        0.0F,
        1.0F
    );
    return glm::distance(point, start + segment * t);
}

} // namespace

AncientGearSetBeamVisual::AncientGearSetBeamVisual(
    const std::string &resource,
    float zIndex
) : Util::GameObject(std::make_shared<Util::Image>(resource), zIndex) {
}

Util::Transform AncientGearSetBeamVisual::GetObjectTransform() const {
    return this->m_Transform;
}

void AncientGearSetBeamVisual::ConfigureBeam(
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
    this->SetAbsoluteScale({
        safeLength / 48.0F,
        thicknessScale
    });
}

AncientGearSet::AncientGearSet(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kAncientGearSetStandSprite,
        kAncientGearSetStandSprite,
        kAncientGearSetDieSprite,
        tracePlayer,
        collisionSystem
    ) {
    this->SetMaxHealth(8);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetColliderSize({28.0F, 28.0F});
    this->m_PlayerSpeed = 0.045F;

    this->m_WarningVisual =
        std::make_shared<AncientGearSetBeamVisual>(kLaserSprite, 3.8F);
    this->m_LaserVisual =
        std::make_shared<AncientGearSetBeamVisual>(kLaserSprite, 6.0F);
    this->m_WarningVisual->SetVisible(false);
    this->m_LaserVisual->SetVisible(false);
}

void AncientGearSet::Update() {
    if (this->IsDead()) {
        this->SetWarningVisible(false);
        this->SetLaserVisible(false);
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
        this->m_MoveIntent = {0.0F, 0.0F};
        this->EnterState(GearState::Idle);
        Character::Update();
        return;
    }

    const glm::vec2 toTarget = target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    const float distanceToTarget = glm::length(toTarget);
    if (distanceToTarget > 0.0001F) {
        this->m_FaceDirection = glm::normalize(toTarget);
    }

    switch (this->m_GearState) {
    case GearState::Idle:
        this->SetWarningVisible(false);
        this->SetLaserVisible(false);
        if (distanceToTarget > kAggroDistance) {
            this->m_MoveIntent = {0.0F, 0.0F};
            break;
        }

        if (distanceToTarget > kNormalLaserMaxLength) {
            this->m_MoveIntent = this->m_FaceDirection;
            break;
        }

        this->m_MoveIntent = {0.0F, 0.0F};
        if (now >= this->m_NextAttackTime) {
            this->StartWarning(target);
        }
        break;

    case GearState::Warning:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->UpdateBeamVisual(this->m_WarningVisual, kWarningThicknessScale);
        if (now - this->m_StateStartTime >= kWarningDurationMs) {
            this->FireLaser();
        }
        break;

    case GearState::Firing:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->UpdateBeamVisual(this->m_LaserVisual, kLaserThicknessScale);
        if (!this->m_LaserDamageApplied) {
            const glm::vec2 start = this->GetAbsoluteTranslation();
            this->DamagePlayersAlongLaser(start, start + this->m_LaserDirection * this->m_LaserLength);
            this->m_LaserDamageApplied = true;
        }
        if (now - this->m_StateStartTime >= kLaserDurationMs) {
            this->EnterState(GearState::Cooldown);
        }
        break;

    case GearState::Cooldown:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->SetWarningVisible(false);
        this->SetLaserVisible(false);
        if (now - this->m_StateStartTime >= kAttackCooldownMs) {
            this->EnterState(GearState::Idle);
        }
        break;
    }

    Character::Update();
}

glm::vec2 AncientGearSet::GetMoveIntent() const {
    return this->m_MoveIntent;
}

glm::vec2 AncientGearSet::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void AncientGearSet::UpdateWeaponPresentation() {
}

std::shared_ptr<Character> AncientGearSet::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

glm::vec2 AncientGearSet::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        return this->m_FaceDirection;
    }

    return glm::normalize(direction);
}

void AncientGearSet::EnterState(GearState state) {
    if (this->m_GearState == state) {
        return;
    }

    this->m_GearState = state;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();

    if (state == GearState::Cooldown) {
        this->m_NextAttackTime = this->m_StateStartTime + kAttackCooldownMs;
    }
}

void AncientGearSet::StartWarning(const std::shared_ptr<Character> &target) {
    this->m_LaserDirection = this->NormalizeOrFallback(
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation()
    );
    this->m_FaceDirection = this->m_LaserDirection;
    this->m_LaserLength = std::min(
        glm::distance(target->GetAbsoluteTranslation(), this->GetAbsoluteTranslation()),
        kNormalLaserMaxLength
    );

    this->EnterState(GearState::Warning);
    this->SetWarningVisible(true);
    this->SetLaserVisible(false);
    this->UpdateBeamVisual(this->m_WarningVisual, kWarningThicknessScale);
}

void AncientGearSet::FireLaser() {
    this->EnterState(GearState::Firing);
    this->m_LaserDamageApplied = false;
    this->SetWarningVisible(false);
    this->SetLaserVisible(true);
    this->UpdateBeamVisual(this->m_LaserVisual, kLaserThicknessScale);
}

void AncientGearSet::DamagePlayersAlongLaser(
    const glm::vec2 &start,
    const glm::vec2 &end
) {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    for (const auto &player : this->m_MapSystem->GetPlayers()) {
        if (player == nullptr || player->IsDead()) {
            continue;
        }

        const float playerRadius =
            std::max(player->GetColliderSize().x, player->GetColliderSize().y) * 0.5F;
        if (DistancePointToSegment(player->GetAbsoluteTranslation(), start, end) <=
            playerRadius + kLaserHitHalfThickness) {
            player->ApplyDamage(kLaserDamage);
            this->KnockbackPlayerFromLaser(*player);
        }
    }
}

void AncientGearSet::KnockbackPlayerFromLaser(Player &player) const {
    glm::vec2 knockbackDirection = this->m_LaserDirection;
    if (glm::length(knockbackDirection) <= 0.0001F) {
        knockbackDirection =
            player.GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    }
    if (glm::length(knockbackDirection) <= 0.0001F) {
        knockbackDirection = {1.0F, 0.0F};
    }

    player.ApplyImpulse(glm::normalize(knockbackDirection) * kLaserKnockbackStrength);
}

void AncientGearSet::UpdateBeamVisual(
    const std::shared_ptr<AncientGearSetBeamVisual> &visual,
    float thicknessScale
) {
    if (visual == nullptr) {
        return;
    }

    visual->ConfigureBeam(
        this->GetAbsoluteTranslation(),
        this->m_LaserDirection,
        this->m_LaserLength,
        thicknessScale
    );
}

void AncientGearSet::SetWarningVisible(bool visible) {
    if (this->m_WarningVisual == nullptr) {
        return;
    }

    if (visible && !this->m_WarningAttached) {
        this->AddChild(this->m_WarningVisual);
        this->m_WarningAttached = true;
    } else if (!visible && this->m_WarningAttached) {
        this->RemoveChild(this->m_WarningVisual);
        this->m_WarningAttached = false;
    }

    this->m_WarningVisual->SetVisible(visible);
}

void AncientGearSet::SetLaserVisible(bool visible) {
    if (this->m_LaserVisual == nullptr) {
        return;
    }

    if (visible && !this->m_LaserAttached) {
        this->AddChild(this->m_LaserVisual);
        this->m_LaserAttached = true;
    } else if (!visible && this->m_LaserAttached) {
        this->RemoveChild(this->m_LaserVisual);
        this->m_LaserAttached = false;
    }

    this->m_LaserVisual->SetVisible(visible);
}
