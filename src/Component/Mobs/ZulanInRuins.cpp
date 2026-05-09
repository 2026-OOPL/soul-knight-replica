#include "Component/Mobs/ZulanInRuins.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <random>

#include <glm/geometric.hpp>

#include "Common/Constants.hpp"
#include "Common/CombatFaction.hpp"
#include "Component/Bullet.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kZulanNormalSprite = {
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsBanner.png"
};

const std::vector<std::string> kZulanAngrySprite = {
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsAngry.png"
};

const std::vector<std::string> kZulanDeathSprite = {
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsDeath.png"
};

const std::array<const char *, 4> kCannonSprites = {
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponTopLeft.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponTopRight.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponBottomLeft.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponBottomRight.png"
};

const std::array<glm::vec2, 4> kCannonHomeOffsets = {
    glm::vec2{-20.0F, 12.0F},
    glm::vec2{20.0F, 12.0F},
    glm::vec2{-18.0F, -10.0F},
    glm::vec2{18.0F, -10.0F}
};

const std::vector<std::string> kBarBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyBarBullet.png"
};

const std::string kLaserResource = RESOURCE_DIR"/Bullet/EnemyLaserBullet.png";

constexpr int kZulanMaxHealth = 600;
constexpr float kZulanMoveSpeed = 0.045F;
constexpr float kZulanRetreatDistance = 120.0F;
constexpr float kZulanPreferredDistance = 170.0F;
constexpr float kZulanApproachDistance = 230.0F;
constexpr float kZulanAggroDistance = 520.0F;
constexpr float kZulanStrafeFlipMinMs = 900.0F;
constexpr float kZulanStrafeFlipMaxMs = 1800.0F;
constexpr float kInitialSkillDelayMs = 700.0F;
constexpr float kAutoChaseCooldownMs = 1300.0F;

constexpr float kCannonMoveSpeed = 0.46F;
constexpr float kCannonStoredZIndex = 3.6F;
constexpr float kCannonActiveZIndex = 5.3F;
constexpr float kCannonArrivalDistance = 8.0F;
constexpr float kCannonAttackIntervalMs = 360.0F;
constexpr float kCannonLaserWarningMs = 560.0F;
constexpr float kCannonLaserDurationMs = 120.0F;
constexpr float kCannonChaseOffsetMin = 56.0F;
constexpr float kCannonChaseOffsetMax = 118.0F;

constexpr float kBreakthroughArrayForwardOffset = 92.0F;
constexpr float kBreakthroughArraySideSpacing = 34.0F;
constexpr float kBreakthroughArrayArcDepth = 10.0F;
constexpr float kRotatingArrayRadius = 86.0F;
constexpr float kRotatingArraySpeedRadiansPerMs = 0.0022F;
constexpr float kArrayFormationDelayMs = 180.0F;
constexpr float kArrayRoundIntervalMs = 1000.0F;
constexpr float kArrayLaserWarningMs = 520.0F;
constexpr float kArrayLaserDurationMs = 120.0F;
constexpr int kBreakthroughArrayRounds = 1;
constexpr int kRotatingArrayRounds = 5;
constexpr int kRotatingArrayReturnAfterRounds = 3;

constexpr int kAutoChaseDamage = 2;
constexpr int kBarBurstCount = 5;
constexpr float kBarBulletSpeed = 0.54F;
constexpr float kBarBulletSpreadRadians = 0.36F;
constexpr float kBarBulletLifetimeMs = 2600.0F;
constexpr float kLaserFallbackLength = MAP_PIXEL_PER_BLOCK * 6.0F;
constexpr float kWarningThicknessScale = 0.07F;
constexpr float kLaserThicknessScale = 0.30F;
constexpr float kLaserHitHalfThickness = 8.0F;

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

glm::vec2 RotateVector(const glm::vec2 &vector, float radians) {
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    return {
        vector.x * cosine - vector.y * sine,
        vector.x * sine + vector.y * cosine
    };
}

class ZulanTimedBullet : public Bullet {
public:
    ZulanTimedBullet(
        const std::vector<std::string> &sprite,
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    ) : Bullet(sprite, coordinate, momentum, 4, damage, faction),
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
    Util::ms_t m_SpawnTime = 0;
    float m_LifetimeMs = 0.0F;
};

} // namespace

ZulanInRuinsCannonVisual::ZulanInRuinsCannonVisual(
    const std::shared_ptr<Core::Drawable> &drawable,
    float zIndex
) : Util::GameObject(drawable, zIndex) {
}

Util::Transform ZulanInRuinsCannonVisual::GetObjectTransform() const {
    return this->m_Transform;
}

ZulanInRuinsBeamVisual::ZulanInRuinsBeamVisual(
    const std::string &resource,
    float zIndex
) : Util::GameObject(std::make_shared<Util::Image>(resource), zIndex) {
}

Util::Transform ZulanInRuinsBeamVisual::GetObjectTransform() const {
    return this->m_Transform;
}

void ZulanInRuinsBeamVisual::ConfigureBeam(
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

ZulanInRuins::ZulanInRuins(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kZulanNormalSprite,
        kZulanNormalSprite,
        kZulanDeathSprite,
        tracePlayer,
        collisionSystem
    ),
    m_RandomEngine(std::random_device{}()) {
    this->m_NormalAnimation = this->m_StandAnimation;
    this->m_AngryAnimation = std::make_shared<Util::Animation>(
        kZulanAngrySprite,
        true,
        20,
        true,
        0,
        false
    );

    this->SetMaxHealth(kZulanMaxHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetColliderSize({86.0F, 78.0F});
    this->SetWeapon(nullptr);
    this->m_PlayerSpeed = kZulanMoveSpeed;

    this->m_Cannons.reserve(kCannonSprites.size());
    for (std::size_t index = 0; index < kCannonSprites.size(); ++index) {
        FloatingCannon cannon;
        cannon.homeOffset = kCannonHomeOffsets[index];
        cannon.position = this->GetAbsoluteTranslation() + cannon.homeOffset;
        cannon.visual = std::make_shared<ZulanInRuinsCannonVisual>(
            std::make_shared<Util::Image>(kCannonSprites[index], false),
            kCannonStoredZIndex
        );
        cannon.warningVisual =
            std::make_shared<ZulanInRuinsBeamVisual>(kLaserResource, 4.6F);
        cannon.laserVisual =
            std::make_shared<ZulanInRuinsBeamVisual>(kLaserResource, 6.0F);
        cannon.warningVisual->SetVisible(false);
        cannon.laserVisual->SetVisible(false);
        this->AddChild(cannon.visual);
        this->m_Cannons.push_back(cannon);
    }

    this->m_NextSkillTime = Util::Time::GetElapsedTimeMs() + kInitialSkillDelayMs;
}

void ZulanInRuins::Update() {
    if (this->IsDead()) {
        this->CleanupCannonVisuals();
        Character::Update();
        return;
    }

    this->UpdatePhase();

    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        Character::Update();
        this->ResetCannonsToHome();
        return;
    }

    this->UpdateState();
    Character::Update();

    for (FloatingCannon &cannon : this->m_Cannons) {
        if (cannon.state == CannonState::Home) {
            cannon.position = this->GetAbsoluteTranslation() + cannon.homeOffset;
        }
        this->UpdateCannonVisual(cannon);
    }
}

void ZulanInRuins::ApplyDamage(int damage) {
    Mob::ApplyDamage(damage);
    this->UpdatePhase();
}

void ZulanInRuins::SetAbsoluteTranslation(glm::vec2 transform) {
    Mob::SetAbsoluteTranslation(transform);
    this->ResetCannonsToHome();
}

glm::vec2 ZulanInRuins::GetMoveIntent() const {
    return this->m_MoveIntent;
}

glm::vec2 ZulanInRuins::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void ZulanInRuins::UpdateWeaponPresentation() {
}

void ZulanInRuins::UpdatePhase() {
    if (this->m_Phase == Phase::Angry || this->IsDead()) {
        return;
    }

    if (this->GetCurrentHealth() * 2 < this->GetMaxHealth()) {
        this->EnterAngryPhase();
    }
}

void ZulanInRuins::EnterAngryPhase() {
    this->m_Phase = Phase::Angry;
    this->m_StandAnimation = this->m_AngryAnimation;
    this->m_WalkAnimation = this->m_AngryAnimation;
    this->SetDrawable(this->m_AngryAnimation);
}

void ZulanInRuins::UpdateState() {
    const std::shared_ptr<Character> target = this->GetTarget();
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (target == nullptr || target->IsDead()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        this->EnterState(BossState::Idle);
        this->SetAllCannonBeamsVisible(false);
        this->ResetCannonsToHome();
        return;
    }

    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    if (glm::length(toTarget) > 0.0001F) {
        this->m_FaceDirection = glm::normalize(toTarget);
    }

    this->UpdateSlowMovement(target);

    switch (this->m_BossState) {
    case BossState::Idle:
        if (glm::length(toTarget) <= kZulanAggroDistance && now >= this->m_NextSkillTime) {
            if (this->m_NextSkill == SkillKind::AutoChase) {
                this->StartAutoChase(target);
            } else {
                this->StartArrayShot(target);
            }
        }
        break;

    case BossState::AutoChase:
        this->UpdateAutoChase(target);
        break;

    case BossState::ArrayShot:
        if (this->m_ArrayFormation == ArrayFormation::Rotating && !this->m_ArrayReturning) {
            this->m_MoveIntent = {0.0F, 0.0F};
        }
        this->UpdateArrayShot(target);
        break;

    case BossState::Cooldown:
        if (now >= this->m_NextSkillTime) {
            this->EnterState(BossState::Idle);
        }
        break;
    }
}

void ZulanInRuins::EnterState(BossState state) {
    if (this->m_BossState == state) {
        return;
    }

    this->m_BossState = state;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();

    if (state == BossState::Cooldown) {
        this->m_NextSkillTime = this->m_StateStartTime + kAutoChaseCooldownMs;
    }
}

void ZulanInRuins::UpdateSlowMovement(const std::shared_ptr<Character> &target) {
    if (target == nullptr) {
        this->m_MoveIntent = {0.0F, 0.0F};
        return;
    }

    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    const float distance = glm::length(toTarget);
    if (distance <= 0.0001F) {
        this->m_MoveIntent = {0.0F, 0.0F};
        return;
    }

    const glm::vec2 towardTarget = glm::normalize(toTarget);
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (now >= this->m_NextStrafeFlipTime) {
        this->m_StrafeDirection = this->RandomBool() ? 1 : -1;
        this->m_NextStrafeFlipTime =
            now + this->RandomFloat(kZulanStrafeFlipMinMs, kZulanStrafeFlipMaxMs);
    }

    if (distance > kZulanApproachDistance) {
        this->m_MoveIntent = towardTarget;
        return;
    }

    if (distance < kZulanRetreatDistance) {
        this->m_MoveIntent = -towardTarget;
        return;
    }

    const glm::vec2 strafeDirection = {
        -towardTarget.y * static_cast<float>(this->m_StrafeDirection),
        towardTarget.x * static_cast<float>(this->m_StrafeDirection)
    };
    const float distanceBias =
        (distance - kZulanPreferredDistance) /
        (kZulanApproachDistance - kZulanRetreatDistance);
    const glm::vec2 radialCorrection =
        towardTarget * std::clamp(distanceBias, -0.45F, 0.45F);
    this->m_MoveIntent = this->NormalizeOrFallback(strafeDirection + radialCorrection);
}

void ZulanInRuins::StartAutoChase(const std::shared_ptr<Character> &target) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();

    for (FloatingCannon &cannon : this->m_Cannons) {
        cannon.state = CannonState::Approaching;
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonActiveZIndex);
        }
        cannon.position = this->GetAbsoluteTranslation() + cannon.homeOffset;
        cannon.chaseOffset = this->RandomChaseOffset();
        cannon.attacksRemaining = this->RandomAttackCount();
        cannon.nextAttackTime = now + this->RandomFloat(120.0F, 340.0F);
        cannon.stateStartTime = now;
        cannon.laserDamageApplied = false;
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);

        if (target != nullptr) {
            cannon.fireDirection =
                this->NormalizeOrFallback(target->GetAbsoluteTranslation() - cannon.position);
        }
    }

    this->EnterState(BossState::AutoChase);
}

void ZulanInRuins::UpdateAutoChase(const std::shared_ptr<Character> &target) {
    for (FloatingCannon &cannon : this->m_Cannons) {
        this->UpdateCannon(cannon, target);
    }

    if (this->AreCannonsHome()) {
        this->m_NextSkill = SkillKind::ArrayShot;
        this->EnterState(BossState::Cooldown);
    }
}

bool ZulanInRuins::AreCannonsHome() const {
    for (const FloatingCannon &cannon : this->m_Cannons) {
        if (cannon.state != CannonState::Home) {
            return false;
        }
    }

    return true;
}

void ZulanInRuins::StartArrayShot(const std::shared_ptr<Character> &target) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    this->m_ArrayFormation =
        this->RandomBool() ? ArrayFormation::Breakthrough : ArrayFormation::Rotating;
    this->m_ArrayAttackMode =
        this->RandomBool() ? ArrayAttackMode::BarBurst : ArrayAttackMode::Laser;
    this->m_ArrayTotalShots =
        this->m_ArrayFormation == ArrayFormation::Breakthrough ?
            kBreakthroughArrayRounds :
            kRotatingArrayRounds;
    this->m_ArrayShotsFired = 0;
    this->m_ArrayReturning = false;
    this->m_ArrayLaserWarningActive = false;
    this->m_ArrayLaserActive = false;
    this->m_ArrayRoundStartTime = now;
    this->m_NextArrayShotTime = now + kArrayFormationDelayMs;
    this->m_ArrayHorizontalDirection = this->m_FaceDirection.x >= 0.0F ? 1.0F : -1.0F;
    this->m_ArrayRotationDirection = this->m_ArrayHorizontalDirection > 0.0F ? 1 : -1;

    if (target != nullptr) {
        const glm::vec2 toTarget =
            target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
        if (glm::length(toTarget) > 0.0001F) {
            this->m_ArrayRotationAngle = std::atan2(toTarget.y, toTarget.x);
        }
    }

    for (FloatingCannon &cannon : this->m_Cannons) {
        cannon.state = CannonState::Approaching;
        cannon.laserDamageApplied = false;
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonActiveZIndex);
        }
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
    }

    this->EnterState(BossState::ArrayShot);
}

void ZulanInRuins::UpdateArrayShot(const std::shared_ptr<Character> &target) {
    if (this->m_ArrayFormation == ArrayFormation::Breakthrough) {
        this->UpdateBreakthroughArrayShot(target);
        return;
    }

    this->UpdateRotatingArrayShot();
}

void ZulanInRuins::UpdateBreakthroughArrayShot(
    const std::shared_ptr<Character> &target
) {
    if (this->IsArrayRoundBusy()) {
        this->TryStartArrayRound(target);
        return;
    }

    if (!this->m_ArrayReturning) {
        const bool ready = this->AreCannonsAtArrayFormation(target);
        if (ready && !this->IsArrayRoundBusy()) {
            this->TryStartArrayRound(target);
        }
        if (this->m_ArrayShotsFired >= this->m_ArrayTotalShots &&
            !this->IsArrayRoundBusy()) {
            this->StartArrayReturn();
        }
        return;
    }

    for (FloatingCannon &cannon : this->m_Cannons) {
        this->MoveCannonToward(
            cannon,
            this->GetAbsoluteTranslation() + cannon.homeOffset
        );
    }
    if (this->AreCannonsAtHome()) {
        this->ResetCannonsToHome();
        this->m_NextSkill = SkillKind::ArrayShot;
        this->EnterState(BossState::Cooldown);
    }
}

void ZulanInRuins::UpdateRotatingArrayShot() {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (this->IsArrayRoundBusy()) {
        this->TryStartArrayRound(this->GetTarget());
        return;
    }

    if (!this->m_ArrayReturning) {
        const float deltaTimeMs = std::min(Util::Time::GetDeltaTimeMs(), 50.0F);
        this->m_ArrayRotationAngle +=
            kRotatingArraySpeedRadiansPerMs *
            deltaTimeMs *
            static_cast<float>(this->m_ArrayRotationDirection);

        const bool ready = this->AreCannonsAtArrayFormation(this->GetTarget()) ||
            now - this->m_StateStartTime >= kArrayFormationDelayMs + 500.0F;
        if (ready && !this->IsArrayRoundBusy()) {
            this->TryStartArrayRound(this->GetTarget());
        }
        if (this->m_ArrayShotsFired >= kRotatingArrayReturnAfterRounds &&
            !this->IsArrayRoundBusy()) {
            this->StartArrayReturn();
        }
        return;
    }

    for (FloatingCannon &cannon : this->m_Cannons) {
        this->MoveCannonToward(
            cannon,
            this->GetAbsoluteTranslation() + cannon.homeOffset
        );
    }
    if (!this->IsArrayRoundBusy()) {
        this->TryStartArrayRound(this->GetTarget());
    }
    if (this->m_ArrayShotsFired >= this->m_ArrayTotalShots &&
        !this->IsArrayRoundBusy() &&
        this->AreCannonsAtHome()) {
        this->ResetCannonsToHome();
        this->m_NextSkill = SkillKind::ArrayShot;
        this->EnterState(BossState::Cooldown);
    }
}

void ZulanInRuins::StartArrayReturn() {
    if (this->m_ArrayReturning) {
        return;
    }

    this->m_ArrayReturning = true;
    for (FloatingCannon &cannon : this->m_Cannons) {
        cannon.state = CannonState::Returning;
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
    }
}

bool ZulanInRuins::AreCannonsAtHome() const {
    for (const FloatingCannon &cannon : this->m_Cannons) {
        if (glm::distance(
                cannon.position,
                this->GetAbsoluteTranslation() + cannon.homeOffset
            ) > kCannonArrivalDistance) {
            return false;
        }
    }

    return true;
}

bool ZulanInRuins::AreCannonsAtArrayFormation(
    const std::shared_ptr<Character> &target
) {
    bool allReady = true;
    for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
        FloatingCannon &cannon = this->m_Cannons[index];
        const glm::vec2 targetPosition =
            this->GetArrayFormationPosition(index, target);
        this->MoveCannonToward(cannon, targetPosition);
        cannon.fireDirection = this->GetArrayFireDirection(index, target);
        if (glm::distance(cannon.position, targetPosition) > kCannonArrivalDistance) {
            allReady = false;
        }
    }

    return allReady;
}

glm::vec2 ZulanInRuins::GetArrayFormationPosition(
    std::size_t index,
    const std::shared_ptr<Character> &target
) const {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
    if (this->m_ArrayFormation == ArrayFormation::Rotating) {
        const float angle =
            this->m_ArrayRotationAngle +
            2.0F * kPi * static_cast<float>(index) /
                static_cast<float>(std::max<std::size_t>(1, this->m_Cannons.size()));
        return origin + glm::vec2(std::cos(angle), std::sin(angle)) * kRotatingArrayRadius;
    }

    glm::vec2 forward = this->m_FaceDirection;
    if (target != nullptr) {
        const glm::vec2 toTarget = target->GetAbsoluteTranslation() - origin;
        if (glm::length(toTarget) > 0.0001F) {
            forward = glm::normalize(toTarget);
        }
    }
    forward = this->NormalizeOrFallback(forward);
    const glm::vec2 side = {-forward.y, forward.x};
    const float centerIndex =
        (static_cast<float>(this->m_Cannons.size()) - 1.0F) * 0.5F;
    const float slot = static_cast<float>(index) - centerIndex;
    return origin +
        forward * (kBreakthroughArrayForwardOffset -
            std::abs(slot) * kBreakthroughArrayArcDepth) +
        side * slot * kBreakthroughArraySideSpacing;
}

glm::vec2 ZulanInRuins::GetArrayFireDirection(
    std::size_t index,
    const std::shared_ptr<Character> &target
) const {
    if (this->m_ArrayFormation == ArrayFormation::Breakthrough) {
        if (target != nullptr) {
            return this->NormalizeOrFallback(
                target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation()
            );
        }
        return this->NormalizeOrFallback(this->m_FaceDirection);
    }

    if (this->m_ArrayReturning) {
        if (this->m_ArrayShotsFired >= 4) {
            return {this->m_ArrayHorizontalDirection, 0.0F};
        }

        const float centerIndex =
            (static_cast<float>(this->m_Cannons.size()) - 1.0F) * 0.5F;
        const float verticalOffset =
            (static_cast<float>(index) - centerIndex) * 0.14F;
        return this->NormalizeOrFallback({
            this->m_ArrayHorizontalDirection,
            verticalOffset
        });
    }

    if (index < this->m_Cannons.size()) {
        const glm::vec2 outward =
            this->m_Cannons[index].position - this->GetAbsoluteTranslation();
        return this->NormalizeOrFallback(outward);
    }

    return this->NormalizeOrFallback(this->m_FaceDirection);
}

void ZulanInRuins::TryStartArrayRound(const std::shared_ptr<Character> &target) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (this->m_ArrayLaserWarningActive) {
        if (now - this->m_ArrayRoundStartTime >= kArrayLaserWarningMs) {
            this->FireArrayLaser();
        }
        return;
    }

    if (this->m_ArrayLaserActive) {
        if (now - this->m_ArrayRoundStartTime >= kArrayLaserDurationMs) {
            this->FinishArrayRound();
        }
        return;
    }

    if (this->m_ArrayShotsFired >= this->m_ArrayTotalShots ||
        now < this->m_NextArrayShotTime) {
        return;
    }

    if (this->m_ArrayAttackMode == ArrayAttackMode::Laser) {
        this->StartArrayLaserWarning(target);
        return;
    }

    for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
        this->FireCannonBarBurstInDirection(
            this->m_Cannons[index],
            this->GetArrayFireDirection(index, target)
        );
    }
    this->FinishArrayRound();
}

void ZulanInRuins::StartArrayLaserWarning(
    const std::shared_ptr<Character> &target
) {
    for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
        this->StartCannonLaserWarningInDirection(
            this->m_Cannons[index],
            this->GetArrayFireDirection(index, target)
        );
    }

    this->m_ArrayLaserWarningActive = true;
    this->m_ArrayRoundStartTime = Util::Time::GetElapsedTimeMs();
}

void ZulanInRuins::FireArrayLaser() {
    for (FloatingCannon &cannon : this->m_Cannons) {
        this->FireCannonLaser(cannon);
        const glm::vec2 laserEnd =
            cannon.position + cannon.fireDirection * cannon.beamLength;
        this->DamagePlayersAlongLaser(cannon.position, laserEnd);
        cannon.laserDamageApplied = true;
    }

    this->m_ArrayLaserWarningActive = false;
    this->m_ArrayLaserActive = true;
    this->m_ArrayRoundStartTime = Util::Time::GetElapsedTimeMs();
}

void ZulanInRuins::FinishArrayRound() {
    this->SetAllCannonBeamsVisible(false);
    this->m_ArrayLaserWarningActive = false;
    this->m_ArrayLaserActive = false;
    this->m_ArrayShotsFired++;
    this->m_NextArrayShotTime =
        Util::Time::GetElapsedTimeMs() + kArrayRoundIntervalMs;

    if (this->m_ArrayFormation == ArrayFormation::Rotating &&
        this->m_ArrayShotsFired >= kRotatingArrayReturnAfterRounds &&
        this->m_ArrayShotsFired < this->m_ArrayTotalShots) {
        this->StartArrayReturn();
    }
}

bool ZulanInRuins::IsArrayRoundBusy() const {
    return this->m_ArrayLaserWarningActive || this->m_ArrayLaserActive;
}

void ZulanInRuins::UpdateCannon(
    FloatingCannon &cannon,
    const std::shared_ptr<Character> &target
) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const glm::vec2 homePosition = this->GetAbsoluteTranslation() + cannon.homeOffset;

    if (target == nullptr || target->IsDead()) {
        cannon.state = CannonState::Returning;
    }

    switch (cannon.state) {
    case CannonState::Home:
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonStoredZIndex);
        }
        cannon.position = homePosition;
        break;

    case CannonState::Approaching: {
        const glm::vec2 chasePosition =
            target == nullptr ? homePosition : target->GetAbsoluteTranslation() + cannon.chaseOffset;
        this->MoveCannonToward(cannon, chasePosition);
        if (glm::distance(cannon.position, chasePosition) <= kCannonArrivalDistance) {
            cannon.state = CannonState::Ready;
            cannon.stateStartTime = now;
            cannon.nextAttackTime = now + 120.0F;
        }
        break;
    }

    case CannonState::Ready: {
        const glm::vec2 chasePosition =
            target == nullptr ? homePosition : target->GetAbsoluteTranslation() + cannon.chaseOffset;
        this->MoveCannonToward(cannon, chasePosition);
        if (cannon.attacksRemaining <= 0) {
            cannon.state = CannonState::Returning;
            cannon.stateStartTime = now;
            break;
        }
        if (now >= cannon.nextAttackTime) {
            this->StartCannonAttack(cannon, target);
        }
        break;
    }

    case CannonState::WarningLaser: {
        const glm::vec2 chasePosition =
            target == nullptr ? homePosition : target->GetAbsoluteTranslation() + cannon.chaseOffset;
        this->MoveCannonToward(cannon, chasePosition);
        if (cannon.warningVisual != nullptr) {
            cannon.warningVisual->ConfigureBeam(
                cannon.position,
                cannon.fireDirection,
                cannon.beamLength,
                kWarningThicknessScale
            );
        }
        if (now - cannon.stateStartTime >= kCannonLaserWarningMs) {
            this->FireCannonLaser(cannon);
        }
        break;
    }

    case CannonState::FiringLaser:
        if (!cannon.laserDamageApplied) {
            const glm::vec2 laserEnd =
                cannon.position + cannon.fireDirection * cannon.beamLength;
            this->DamagePlayersAlongLaser(cannon.position, laserEnd);
            cannon.laserDamageApplied = true;
        }
        if (cannon.laserVisual != nullptr) {
            cannon.laserVisual->ConfigureBeam(
                cannon.position,
                cannon.fireDirection,
                cannon.beamLength,
                kLaserThicknessScale
            );
        }
        if (now - cannon.stateStartTime >= kCannonLaserDurationMs) {
            this->FinishCannonAttack(cannon);
        }
        break;

    case CannonState::Returning:
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonStoredZIndex);
        }
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
        this->MoveCannonToward(cannon, homePosition);
        if (glm::distance(cannon.position, homePosition) <= kCannonArrivalDistance) {
            cannon.position = homePosition;
            cannon.state = CannonState::Home;
        }
        break;
    }
}

void ZulanInRuins::UpdateCannonVisual(FloatingCannon &cannon) {
    if (cannon.visual == nullptr) {
        return;
    }

    cannon.visual->SetAbsoluteTranslation(cannon.position);
    const glm::vec2 direction = this->NormalizeOrFallback(cannon.fireDirection);
    cannon.visual->SetAbsoluteRotation(std::atan2(direction.y, direction.x));
}

void ZulanInRuins::MoveCannonToward(
    FloatingCannon &cannon,
    const glm::vec2 &targetPosition
) {
    const glm::vec2 toTarget = targetPosition - cannon.position;
    const float distance = glm::length(toTarget);
    if (distance <= 0.0001F) {
        return;
    }

    const float deltaTimeMs = std::min(Util::Time::GetDeltaTimeMs(), 50.0F);
    const float step = kCannonMoveSpeed * deltaTimeMs;
    if (step >= distance) {
        cannon.position = targetPosition;
        return;
    }

    cannon.position += glm::normalize(toTarget) * step;
}

void ZulanInRuins::StartCannonAttack(
    FloatingCannon &cannon,
    const std::shared_ptr<Character> &target
) {
    if (target == nullptr || target->IsDead()) {
        cannon.state = CannonState::Returning;
        return;
    }

    cannon.fireDirection =
        this->NormalizeOrFallback(target->GetAbsoluteTranslation() - cannon.position);

    if (this->RandomBool()) {
        this->FireCannonBarBurst(cannon, target);
        this->FinishCannonAttack(cannon);
        return;
    }

    this->StartCannonLaserWarning(cannon, target);
}

void ZulanInRuins::FireCannonBarBurst(
    FloatingCannon &cannon,
    const std::shared_ptr<Character> &target
) {
    if (this->m_MapSystem == nullptr || target == nullptr) {
        return;
    }

    const glm::vec2 baseDirection =
        this->NormalizeOrFallback(target->GetAbsoluteTranslation() - cannon.position);
    this->FireCannonBarBurstInDirection(cannon, baseDirection);
}

void ZulanInRuins::FireCannonBarBurstInDirection(
    FloatingCannon &cannon,
    const glm::vec2 &direction
) {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 baseDirection = this->NormalizeOrFallback(direction);
    cannon.fireDirection = baseDirection;

    for (int index = 0; index < kBarBurstCount; ++index) {
        const float angle =
            this->RandomFloat(-kBarBulletSpreadRadians, kBarBulletSpreadRadians);
        const glm::vec2 direction = this->NormalizeOrFallback(
            RotateVector(baseDirection, angle)
        );

        std::shared_ptr<Bullet> bullet = std::make_shared<ZulanTimedBullet>(
            kBarBulletSprite,
            cannon.position + direction * 20.0F,
            direction * kBarBulletSpeed,
            kAutoChaseDamage,
            this->GetFaction(),
            kBarBulletLifetimeMs
        );
        bullet->SetColliderSize({18.0F, 8.0F});
        this->m_MapSystem->AddBullet(bullet);
    }
}

void ZulanInRuins::StartCannonLaserWarning(
    FloatingCannon &cannon,
    const std::shared_ptr<Character> &target
) {
    if (target == nullptr) {
        cannon.state = CannonState::Returning;
        return;
    }

    this->StartCannonLaserWarningInDirection(
        cannon,
        target->GetAbsoluteTranslation() - cannon.position
    );
}

void ZulanInRuins::StartCannonLaserWarningInDirection(
    FloatingCannon &cannon,
    const glm::vec2 &direction
) {
    cannon.fireDirection = this->NormalizeOrFallback(direction);
    cannon.beamLength = this->ComputeRoomClippedBeamLength(
        cannon.position,
        cannon.fireDirection,
        kLaserFallbackLength
    );
    cannon.laserDamageApplied = false;
    cannon.state = CannonState::WarningLaser;
    cannon.stateStartTime = Util::Time::GetElapsedTimeMs();

    if (cannon.warningVisual != nullptr) {
        cannon.warningVisual->ConfigureBeam(
            cannon.position,
            cannon.fireDirection,
            cannon.beamLength,
            kWarningThicknessScale
        );
    }
    this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, true);
    this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
}

void ZulanInRuins::FireCannonLaser(FloatingCannon &cannon) {
    cannon.state = CannonState::FiringLaser;
    cannon.stateStartTime = Util::Time::GetElapsedTimeMs();
    cannon.laserDamageApplied = false;

    this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
    if (cannon.laserVisual != nullptr) {
        cannon.laserVisual->ConfigureBeam(
            cannon.position,
            cannon.fireDirection,
            cannon.beamLength,
            kLaserThicknessScale
        );
    }
    this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, true);
}

void ZulanInRuins::FinishCannonAttack(FloatingCannon &cannon) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    cannon.attacksRemaining--;
    cannon.nextAttackTime = now + kCannonAttackIntervalMs;
    cannon.stateStartTime = now;
    this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
    this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);

    if (cannon.attacksRemaining <= 0) {
        cannon.state = CannonState::Returning;
        return;
    }

    cannon.chaseOffset = this->RandomChaseOffset();
    cannon.state = CannonState::Ready;
}

void ZulanInRuins::SetBeamVisible(
    FloatingCannon &,
    const std::shared_ptr<ZulanInRuinsBeamVisual> &visual,
    bool &attached,
    bool visible
) {
    if (visual == nullptr) {
        return;
    }

    if (visible && !attached) {
        this->AddChild(visual);
        attached = true;
    } else if (!visible && attached) {
        this->RemoveChild(visual);
        attached = false;
    }

    visual->SetVisible(visible);
}

void ZulanInRuins::SetAllCannonBeamsVisible(bool visible) {
    for (FloatingCannon &cannon : this->m_Cannons) {
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, visible);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, visible);
    }
}

void ZulanInRuins::ResetCannonsToHome() {
    for (FloatingCannon &cannon : this->m_Cannons) {
        cannon.state = CannonState::Home;
        cannon.position = this->GetAbsoluteTranslation() + cannon.homeOffset;
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonStoredZIndex);
        }
        this->UpdateCannonVisual(cannon);
    }
}

void ZulanInRuins::CleanupCannonVisuals() {
    if (this->m_CannonVisualsCleaned) {
        return;
    }

    for (FloatingCannon &cannon : this->m_Cannons) {
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
        if (cannon.visual != nullptr) {
            this->RemoveChild(cannon.visual);
            cannon.visual->SetVisible(false);
        }
    }

    this->m_CannonVisualsCleaned = true;
}

void ZulanInRuins::DamagePlayersAlongLaser(
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
            player->ApplyDamage(kAutoChaseDamage);
        }
    }
}

float ZulanInRuins::ComputeRoomClippedBeamLength(
    const glm::vec2 &start,
    const glm::vec2 &direction,
    float fallbackLength
) const {
    const glm::vec2 forward = this->NormalizeOrFallback(direction);
    if (this->m_MapSystem == nullptr) {
        return fallbackLength;
    }

    const std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room == nullptr) {
        return fallbackLength;
    }

    const glm::vec2 roomCenter = room->GetAbsoluteTranslation();
    const glm::vec2 roomHalfSize = room->GetAreaSize() * 0.5F;
    constexpr float kRoomEdgePadding = 8.0F;

    const float left = roomCenter.x - roomHalfSize.x + kRoomEdgePadding;
    const float right = roomCenter.x + roomHalfSize.x - kRoomEdgePadding;
    const float bottom = roomCenter.y - roomHalfSize.y + kRoomEdgePadding;
    const float top = roomCenter.y + roomHalfSize.y - kRoomEdgePadding;

    float maxLength = fallbackLength;
    if (forward.x > 0.0001F) {
        maxLength = std::min(maxLength, (right - start.x) / forward.x);
    } else if (forward.x < -0.0001F) {
        maxLength = std::min(maxLength, (left - start.x) / forward.x);
    }

    if (forward.y > 0.0001F) {
        maxLength = std::min(maxLength, (top - start.y) / forward.y);
    } else if (forward.y < -0.0001F) {
        maxLength = std::min(maxLength, (bottom - start.y) / forward.y);
    }

    return std::max(1.0F, maxLength);
}

std::shared_ptr<Character> ZulanInRuins::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

glm::vec2 ZulanInRuins::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        if (glm::length(this->m_FaceDirection) > 0.0001F) {
            return glm::normalize(this->m_FaceDirection);
        }
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

glm::vec2 ZulanInRuins::RandomChaseOffset() {
    const float angle = this->RandomFloat(0.0F, 2.0F * kPi);
    const float radius = this->RandomFloat(kCannonChaseOffsetMin, kCannonChaseOffsetMax);
    return {std::cos(angle) * radius, std::sin(angle) * radius};
}

int ZulanInRuins::RandomAttackCount() {
    const int maxExtra = this->m_Phase == Phase::Angry ? 2 : 1;
    std::uniform_int_distribution<int> distribution(0, maxExtra);
    return 2 + distribution(this->m_RandomEngine);
}

float ZulanInRuins::RandomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}

bool ZulanInRuins::RandomBool() {
    std::bernoulli_distribution distribution(0.5);
    return distribution(this->m_RandomEngine);
}
