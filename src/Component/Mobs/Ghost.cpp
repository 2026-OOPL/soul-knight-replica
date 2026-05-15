#include "Component/Mobs/Ghost.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <vector>

#include <glm/geometric.hpp>

#include "Common/CombatFaction.hpp"
#include "Component/Bullet.hpp"
#include "Component/Bullets/TimedBullet.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kGhostStandSprite = {
    RESOURCE_DIR"/Mob/Ghost/Ghost_stand_0.png"
};

const std::vector<std::string> kGhostWalkSprite = {
    RESOURCE_DIR"/Mob/Ghost/Ghost_walk_0.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_walk_1.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_walk_2.png"
};

const std::vector<std::string> kGhostDieSprite = {
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_0.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_1.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_2.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_3.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_4.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_5.png",
    RESOURCE_DIR"/Mob/Ghost/Ghost_die_6.png"
};

const std::vector<std::string> kRoundBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyRoundBullet.png"
};

constexpr int kGhostHealth = 9;
constexpr float kGhostMoveSpeed = 0.055F;
constexpr float kGhostRetreatDistance = 82.0F;
constexpr float kGhostPreferredDistance = 128.0F;
constexpr float kGhostApproachDistance = 188.0F;
constexpr float kStrafeFlipMinMs = 800.0F;
constexpr float kStrafeFlipMaxMs = 1500.0F;
constexpr float kAttackInitialMinMs = 700.0F;
constexpr float kAttackInitialMaxMs = 1300.0F;
constexpr float kAttackCooldownMinMs = 1500.0F;
constexpr float kAttackCooldownMaxMs = 2300.0F;
constexpr float kPostAttackVisibleMs = 1000.0F;
constexpr float kHiddenMinMs = 850.0F;
constexpr float kHiddenMaxMs = 1300.0F;
constexpr int kRadialBulletCount = 17;
constexpr int kRadialBulletDamage = 3;
constexpr float kRadialBulletSpeed = 0.32F;
constexpr float kRadialBulletLifetimeMs = 2600.0F;
constexpr float kRadialBulletScale = 0.72F;
const glm::vec2 kRadialBulletColliderSize = {18.0F, 18.0F};
const glm::vec2 kGhostColliderSize = {24.0F, 24.0F};
constexpr float kPi = 3.14159265358979323846F;
constexpr float kTau = kPi * 2.0F;

BulletConfig BuildGhostRoundBulletConfig() {
    BulletConfig config;
    config.sprites = kRoundBulletSprite;
    config.visualScale = {kRadialBulletScale, kRadialBulletScale};
    config.colliderSize = kRadialBulletColliderSize;
    config.zIndex = 5;
    config.loopAnimation = true;
    config.frameIntervalMs = 20;
    return config;
}

} // namespace

Ghost::Ghost(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kGhostStandSprite,
        kGhostWalkSprite,
        kGhostDieSprite,
        tracePlayer,
        collisionSystem
    ),
    m_RandomEngine(std::random_device{}()) {
    this->SetMaxHealth(kGhostHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetColliderSize(kGhostColliderSize);
    this->m_PlayerSpeed = kGhostMoveSpeed;
    this->m_NextAttackTime =
        Util::Time::GetElapsedTimeMs() +
        this->RandomFloat(kAttackInitialMinMs, kAttackInitialMaxMs);
}

void Ghost::Update() {
    if (this->IsDead()) {
        this->SetHidden(false);
        Character::Update();
        return;
    }

    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        Character::Update();
        return;
    }

    const std::shared_ptr<Character> target = this->GetTarget();
    this->UpdateMovement(target);

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    switch (this->m_GhostState) {
    case GhostState::Active:
        this->SetHidden(false);
        if (target != nullptr &&
            !target->IsDead() &&
            now >= this->m_NextAttackTime) {
            this->FireRadialBarrage();
            this->m_GhostState = GhostState::AttackRecover;
            this->m_StateStartTime = now;
            this->m_MoveIntent = {0.0F, 0.0F};
            this->TriggerAttackVisual(160.0F);
        }
        break;

    case GhostState::AttackRecover:
        this->SetHidden(false);
        this->m_MoveIntent = {0.0F, 0.0F};
        if (now - this->m_StateStartTime >= kPostAttackVisibleMs) {
            this->EnterHidden();
        }
        break;

    case GhostState::Hidden:
        this->SetHidden(true);
        if (now >= this->m_HiddenEndTime) {
            this->SetHidden(false);
            this->m_GhostState = GhostState::Active;
            this->m_NextAttackTime =
                now + this->RandomFloat(kAttackCooldownMinMs, kAttackCooldownMaxMs);
        }
        break;
    }

    Character::Update();
}

glm::vec2 Ghost::GetMoveIntent() const {
    return this->m_MoveIntent;
}

glm::vec2 Ghost::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void Ghost::UpdateWeaponPresentation() {
}

std::shared_ptr<Character> Ghost::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

void Ghost::UpdateMovement(const std::shared_ptr<Character> &target) {
    if (target == nullptr || target->IsDead()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        return;
    }

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (now >= this->m_NextStrafeFlipTime) {
        this->m_StrafeDirection = this->RandomInt(0, 1) == 0 ? -1 : 1;
        this->m_NextStrafeFlipTime =
            now + this->RandomFloat(kStrafeFlipMinMs, kStrafeFlipMaxMs);
    }

    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    const float distance = glm::length(toTarget);
    if (distance <= 0.0001F) {
        this->m_MoveIntent = {0.0F, 0.0F};
        return;
    }

    const glm::vec2 forward = glm::normalize(toTarget);
    const glm::vec2 side = {-forward.y, forward.x};
    this->m_FaceDirection = forward;

    if (distance > kGhostApproachDistance) {
        this->m_MoveIntent = forward;
        return;
    }

    if (distance < kGhostRetreatDistance) {
        this->m_MoveIntent = -forward;
        return;
    }

    const float holdBias =
        distance < kGhostPreferredDistance ? -0.24F :
        distance > kGhostPreferredDistance ? 0.20F :
        0.0F;
    this->m_MoveIntent = this->NormalizeOrFallback(
        side * static_cast<float>(this->m_StrafeDirection) + forward * holdBias
    );
}

void Ghost::FireRadialBarrage() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const BulletConfig config = BuildGhostRoundBulletConfig();
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    const float angleOffset = this->RandomFloat(0.0F, kTau);

    for (int index = 0; index < kRadialBulletCount; ++index) {
        const float angle =
            angleOffset +
            kTau * static_cast<float>(index) /
                static_cast<float>(kRadialBulletCount);
        const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
        std::shared_ptr<Bullet> bullet = std::make_shared<TimedBullet>(
            config,
            origin + direction * 12.0F,
            direction * kRadialBulletSpeed,
            kRadialBulletDamage,
            this->GetFaction(),
            kRadialBulletLifetimeMs
        );
        this->m_MapSystem->AddBullet(bullet);
    }
}

void Ghost::EnterHidden() {
    this->m_GhostState = GhostState::Hidden;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_HiddenEndTime =
        this->m_StateStartTime + this->RandomFloat(kHiddenMinMs, kHiddenMaxMs);
    this->SetHidden(true);
}

void Ghost::SetHidden(bool hidden) {
    if (this->m_Hidden == hidden) {
        return;
    }

    this->m_Hidden = hidden;
    this->SetVisible(!hidden);
    this->SetTargetable(!hidden);
}

glm::vec2 Ghost::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        return this->m_FaceDirection;
    }

    return glm::normalize(direction);
}

int Ghost::RandomInt(int minValue, int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}

float Ghost::RandomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}
