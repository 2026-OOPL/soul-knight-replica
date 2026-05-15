#include "Component/Mobs/GhostKing.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <random>
#include <vector>

#include <glm/geometric.hpp>

#include "Common/CombatFaction.hpp"
#include "Common/Constants.hpp"
#include "Component/Bullet.hpp"
#include "Component/Bullets/TimedBullet.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Mobs/Ghost.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kGhostKingStandSprite = {
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_stand_0.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_stand_1.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_stand_2.png"
};

const std::vector<std::string> kGhostKingAttackSprite = {
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_attack_0.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_attack_1.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_attack_2.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_attack_3.png"
};

const std::vector<std::string> kGhostKingPressureSprite = {
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_0.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_1.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_2.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_3.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_4.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_5.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_pressure_6.png"
};

const std::vector<std::string> kGhostKingDisappearSprite = {
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_0.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_1.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_2.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_3.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_4.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_5.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_6.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_7.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_8.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_9.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_10.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_11.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_12.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_13.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_14.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_15.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_16.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_17.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_18.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_19.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_20.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_21.png",
    RESOURCE_DIR"/Boss/GhostKing/GhostKing_disappear_22.png"
};

const std::vector<std::string> kBarBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyBarBullet.png"
};

const std::vector<std::string> kBouncingBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyBouncingBullet.png"
};

const std::vector<std::string> kCrossBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyCrossBullet.png"
};

const std::string kWarningSignSprite = RESOURCE_DIR"/Effect/WarningSign.png";

constexpr int kGhostKingMaxHealth = 600;
constexpr float kBossScale = 1.30F;
const glm::vec2 kBossColliderSize = {48.0F, 58.0F};
constexpr float kBossMoveSpeed = 0.034F;
constexpr float kRetreatDistance = 150.0F;
constexpr float kPreferredDistance = 210.0F;
constexpr float kApproachDistance = 285.0F;
constexpr float kStrafeFlipMinMs = 900.0F;
constexpr float kStrafeFlipMaxMs = 1700.0F;
constexpr float kIntroHiddenMs = 2300.0F;
constexpr float kInitialSkillDelayMs = 700.0F;
constexpr float kSkillCooldownMs = 1050.0F;
constexpr float kRelocationChance = 0.35F;
constexpr float kRelocationWarningMs = 650.0F;
constexpr float kRelocationHiddenMs = 900.0F;
constexpr float kRoomSpawnMargin = 46.0F;
constexpr int kMaxSpawnAttempts = 24;

constexpr int kEyeVolleyGroups = 7;
constexpr float kEyeVolleyIntervalMs = 145.0F;
constexpr float kEyeVolleyAngleStep = 18.0F;
constexpr float kEyeVolleyBulletSpeed = 0.44F;
constexpr float kEyeVolleyBulletLifetimeMs = 3400.0F;
constexpr int kEyeVolleyNormalDamage = 4;
constexpr int kEyeVolleyAngryDamage = 5;
constexpr float kEyeVolleyNormalScale = 0.90F;
constexpr float kEyeVolleyAngryScale = 1.20F;
const glm::vec2 kEyeVolleyNormalColliderSize = {20.0F, 8.0F};
const glm::vec2 kEyeVolleyAngryColliderSize = {26.0F, 10.0F};

constexpr int kScarletNormalShots = 10;
constexpr int kScarletAngryShots = 14;
constexpr float kScarletIntervalMs = 150.0F;
constexpr float kScarletBulletSpeed = 0.28F;
constexpr float kScarletBulletLifetimeMs = 4500.0F;
constexpr int kScarletDamage = 4;
constexpr float kScarletScale = 0.72F;
const glm::vec2 kScarletColliderSize = {20.0F, 20.0F};

constexpr int kSpiritBurstBulletCount = 19;
constexpr float kSpiritBurstIntervalMs = 280.0F;
constexpr float kSpiritBurstBulletSpeed = 0.35F;
constexpr float kSpiritBurstBulletLifetimeMs = 2900.0F;
constexpr int kSpiritBurstDamage = 4;
constexpr float kSpiritBurstScale = 0.80F;
const glm::vec2 kSpiritBurstColliderSize = {20.0F, 20.0F};

constexpr float kPi = 3.14159265358979323846F;
constexpr float kTau = kPi * 2.0F;

constexpr float ToRadians(float degrees) {
    return degrees * kPi / 180.0F;
}

BulletConfig BuildBulletConfig(
    const std::vector<std::string> &sprites,
    float scale,
    const glm::vec2 &colliderSize,
    int zIndex
) {
    BulletConfig config;
    config.sprites = sprites;
    config.visualScale = {scale, scale};
    config.colliderSize = colliderSize;
    config.zIndex = zIndex;
    config.loopAnimation = true;
    config.frameIntervalMs = 20;
    return config;
}

class GhostKingBouncingBullet : public Bullet {
public:
    GhostKingBouncingBullet(
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    ) : Bullet(kBouncingBulletSprite, coordinate, momentum, 5, damage, faction),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()),
        m_LifetimeMs(lifetimeMs) {
        this->SetVisualScalePreservingFlip({kScarletScale, kScarletScale});
        this->SetColliderSize(kScarletColliderSize);
    }

    void Update() override {
        if (this->IsDestroyRequested()) {
            return;
        }

        const float elapsedMs =
            Util::Time::GetElapsedTimeMs() - this->m_SpawnTime;
        if (elapsedMs >= this->m_LifetimeMs) {
            this->RequestDestroy();
            return;
        }

        const float movementDeltaTimeMs =
            std::min(Util::Time::GetDeltaTimeMs(), 10.0F);
        const glm::vec2 frameDelta =
            this->m_Momentum * 0.5F * movementDeltaTimeMs;

        if (this->m_CollisionResolver == nullptr) {
            Bullet::Update();
            return;
        }

        const Collision::MovementResult movementResult =
            this->m_CollisionResolver(*this, frameDelta);
        this->SetAbsoluteTranslation(
            this->GetAbsoluteTranslation() + movementResult.resolvedDelta
        );

        if (movementResult.blockedX) {
            this->m_Momentum.x = -this->m_Momentum.x;
        }
        if (movementResult.blockedY) {
            this->m_Momentum.y = -this->m_Momentum.y;
        }

        if (movementResult.blockedX || movementResult.blockedY) {
            this->SetAbsoluteRotation(
                std::atan2(this->m_Momentum.y, this->m_Momentum.x)
            );
        }
    }

private:
    Util::ms_t m_SpawnTime = 0;
    float m_LifetimeMs = 0.0F;
};

} // namespace

class GhostKingWarningSign : public Util::GameObject, public MapObject {
public:
    GhostKingWarningSign()
        : Util::GameObject(std::make_shared<Util::Image>(kWarningSignSprite), 6.5F) {
        this->SetAbsoluteScale({1.1F, 1.1F});
    }

    Util::Transform GetObjectTransform() const override {
        return this->m_Transform;
    }
};

GhostKing::GhostKing(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kGhostKingStandSprite,
        kGhostKingStandSprite,
        kGhostKingDisappearSprite,
        tracePlayer,
        collisionSystem
    ),
    m_RandomEngine(std::random_device{}()) {
    this->m_NormalAnimation = this->m_StandAnimation;
    this->m_AttackAnimation = std::make_shared<Util::Animation>(
        kGhostKingAttackSprite,
        true,
        70,
        true,
        0,
        false
    );
    this->m_PressureAnimation = std::make_shared<Util::Animation>(
        kGhostKingPressureSprite,
        true,
        55,
        true,
        0,
        false
    );

    this->SetMaxHealth(kGhostKingMaxHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetAbsoluteScale({kBossScale, kBossScale});
    this->SetColliderSize(kBossColliderSize);
    this->SetWeapon(nullptr);
    this->m_PlayerSpeed = kBossMoveSpeed;
    this->SetAttackAnimation(this->m_AttackAnimation);

    this->m_RelocationWarningVisual =
        std::make_shared<GhostKingWarningSign>();
    this->m_RelocationWarningVisual->SetVisible(false);
    this->SetIncorporeal(true);
}

void GhostKing::Update() {
    if (this->IsDead()) {
        this->SetIncorporeal(false);
        this->ShowRelocationWarning(false);
        Character::Update();
        return;
    }

    this->UpdatePhase();

    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        Character::Update();
        return;
    }

    if (!this->m_BattleStarted) {
        this->StartIntroHidden();
    }

    this->UpdateState();
    Character::Update();
}

void GhostKing::ApplyDamage(int damage) {
    Mob::ApplyDamage(damage);
    this->UpdatePhase();
}

glm::vec2 GhostKing::GetMoveIntent() const {
    return this->m_MoveIntent;
}

glm::vec2 GhostKing::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void GhostKing::UpdateWeaponPresentation() {
}

void GhostKing::StartIntroHidden() {
    this->m_BattleStarted = true;
    this->m_BossState = BossState::IntroHidden;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_StateEndTime = this->m_StateStartTime + kIntroHiddenMs;
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetIncorporeal(true);
}

void GhostKing::FinishIntroHidden() {
    this->SetIncorporeal(false);
    this->SpawnGhosts(3);
    this->StartCooldown();
}

void GhostKing::UpdatePhase() {
    if (this->m_Phase == Phase::Angry || this->IsDead()) {
        return;
    }

    if (this->GetCurrentHealth() * 2 <= this->GetMaxHealth()) {
        this->EnterAngryPhase();
    }
}

void GhostKing::EnterAngryPhase() {
    this->m_Phase = Phase::Angry;
}

void GhostKing::UpdateState() {
    const std::shared_ptr<Character> target = this->GetTarget();
    if (target != nullptr && !target->IsDead()) {
        const glm::vec2 toTarget =
            target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
        if (glm::length(toTarget) > 0.0001F) {
            this->m_FaceDirection = glm::normalize(toTarget);
        }
    }

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    switch (this->m_BossState) {
    case BossState::IntroHidden:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->SetIncorporeal(true);
        if (now >= this->m_StateEndTime) {
            this->FinishIntroHidden();
        }
        break;

    case BossState::Idle:
        this->SetIncorporeal(false);
        this->UpdateSlowMovement(target);
        if (now >= this->m_NextSkillTime) {
            this->m_NextSkill = this->RandomSkillKind();
            switch (this->m_NextSkill) {
            case SkillKind::EyeVolley:
                this->EnterState(BossState::EyeVolley);
                break;
            case SkillKind::ScarletFlame:
                this->EnterState(BossState::ScarletFlame);
                break;
            case SkillKind::SpiritBurst:
                this->EnterState(BossState::SpiritBurst);
                break;
            }
        }
        break;

    case BossState::Cooldown:
        this->SetIncorporeal(false);
        this->UpdateSlowMovement(target);
        if (now >= this->m_NextSkillTime) {
            this->EnterState(BossState::Idle);
        }
        break;

    case BossState::RelocationWarning:
        this->UpdateRelocationWarning();
        break;

    case BossState::RelocationHidden:
        this->UpdateRelocationHidden();
        break;

    case BossState::EyeVolley:
        this->UpdateEyeVolley();
        break;

    case BossState::ScarletFlame:
        this->UpdateScarletFlame();
        break;

    case BossState::SpiritBurst:
        this->UpdateSpiritBurst();
        break;
    }
}

void GhostKing::EnterState(BossState state) {
    this->m_BossState = state;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_MoveIntent = {0.0F, 0.0F};
    this->ShowRelocationWarning(false);

    const std::shared_ptr<Character> target = this->GetTarget();
    switch (state) {
    case BossState::IntroHidden:
        this->StartIntroHidden();
        break;

    case BossState::Idle:
        break;

    case BossState::Cooldown:
        this->StartCooldown();
        break;

    case BossState::RelocationWarning:
        this->StartRelocation();
        break;

    case BossState::RelocationHidden:
        break;

    case BossState::EyeVolley:
        this->StartEyeVolley(target);
        break;

    case BossState::ScarletFlame:
        this->StartScarletFlame(target);
        break;

    case BossState::SpiritBurst:
        this->StartSpiritBurst();
        break;
    }
}

void GhostKing::StartCooldown() {
    this->m_BossState = BossState::Cooldown;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_NextSkillTime = this->m_StateStartTime + kSkillCooldownMs;
    this->SetIncorporeal(false);
    this->ShowRelocationWarning(false);
}

void GhostKing::StartCooldownOrRelocate() {
    if (this->RandomFloat(0.0F, 1.0F) < kRelocationChance) {
        this->EnterState(BossState::RelocationWarning);
        return;
    }

    this->StartCooldown();
}

void GhostKing::StartRelocation() {
    this->m_BossState = BossState::RelocationWarning;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_StateEndTime = this->m_StateStartTime + kRelocationWarningMs;
    this->m_RelocationTarget = this->FindSafeBossPosition();
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetIncorporeal(true);
    this->ShowRelocationWarning(true);
}

void GhostKing::UpdateRelocationWarning() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetIncorporeal(true);
    this->ShowRelocationWarning(true);

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (now < this->m_StateEndTime) {
        return;
    }

    this->SetAbsoluteTranslation(this->m_RelocationTarget);
    this->SpawnGhostAt(this->FindSafeGhostPosition(this->m_RelocationTarget));
    this->ShowRelocationWarning(false);
    this->m_BossState = BossState::RelocationHidden;
    this->m_StateStartTime = now;
    this->m_StateEndTime = now + kRelocationHiddenMs;
}

void GhostKing::UpdateRelocationHidden() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetIncorporeal(true);

    if (Util::Time::GetElapsedTimeMs() >= this->m_StateEndTime) {
        this->StartCooldown();
    }
}

void GhostKing::SetIncorporeal(bool incorporeal) {
    if (this->m_Incorporeal == incorporeal) {
        return;
    }

    this->m_Incorporeal = incorporeal;
    this->SetVisible(!incorporeal);
    this->SetTargetable(!incorporeal);
}

void GhostKing::UpdateSlowMovement(const std::shared_ptr<Character> &target) {
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

    if (distance > kApproachDistance) {
        this->m_MoveIntent = forward;
        return;
    }

    if (distance < kRetreatDistance) {
        this->m_MoveIntent = -forward;
        return;
    }

    const float holdBias =
        distance < kPreferredDistance ? -0.25F :
        distance > kPreferredDistance ? 0.20F :
        0.0F;
    this->m_MoveIntent = this->NormalizeOrFallback(
        side * static_cast<float>(this->m_StrafeDirection) + forward * holdBias
    );
}

void GhostKing::StartEyeVolley(const std::shared_ptr<Character> &target) {
    this->SetIncorporeal(false);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_EyeVolleyGroupsFired = 0;
    this->m_NextSkillActionTime = Util::Time::GetElapsedTimeMs();
    this->m_LockedSkillDirection =
        target == nullptr ?
        this->NormalizeOrFallback(this->m_FaceDirection) :
        this->NormalizeOrFallback(
            target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation()
        );
    this->m_FaceDirection = this->m_LockedSkillDirection;
    this->SetAttackAnimation(this->m_AttackAnimation);
    this->TriggerAttackVisual(180.0F);
}

void GhostKing::UpdateEyeVolley() {
    this->m_MoveIntent = {0.0F, 0.0F};

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    while (this->m_EyeVolleyGroupsFired < kEyeVolleyGroups &&
           now >= this->m_NextSkillActionTime) {
        this->FireEyeVolleyGroup();
        this->m_EyeVolleyGroupsFired++;
        this->m_NextSkillActionTime += kEyeVolleyIntervalMs;
        this->TriggerAttackVisual(160.0F);
    }

    if (this->m_EyeVolleyGroupsFired >= kEyeVolleyGroups) {
        this->StartCooldownOrRelocate();
    }
}

void GhostKing::FireEyeVolleyGroup() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const float offsetIndex =
        static_cast<float>(this->m_EyeVolleyGroupsFired) -
        static_cast<float>(kEyeVolleyGroups - 1) * 0.5F;
    const glm::vec2 direction = this->NormalizeOrFallback(
        this->RotateVector(
            this->m_LockedSkillDirection,
            ToRadians(offsetIndex * kEyeVolleyAngleStep)
        )
    );
    const glm::vec2 side = {-direction.y, direction.x};
    const float scale =
        this->m_Phase == Phase::Angry ?
            kEyeVolleyAngryScale :
            kEyeVolleyNormalScale;
    const glm::vec2 colliderSize =
        this->m_Phase == Phase::Angry ?
            kEyeVolleyAngryColliderSize :
            kEyeVolleyNormalColliderSize;
    const int damage =
        this->m_Phase == Phase::Angry ?
            kEyeVolleyAngryDamage :
            kEyeVolleyNormalDamage;
    const BulletConfig config = BuildBulletConfig(
        kBarBulletSprite,
        scale,
        colliderSize,
        5
    );
    const glm::vec2 origin = this->GetFaceFireOrigin();

    for (float sideSign : {-1.0F, 1.0F}) {
        const glm::vec2 bulletOrigin =
            origin + side * (sideSign * 7.0F) + direction * 16.0F;
        std::shared_ptr<Bullet> bullet = std::make_shared<TimedBullet>(
            config,
            bulletOrigin,
            direction * kEyeVolleyBulletSpeed,
            damage,
            this->GetFaction(),
            kEyeVolleyBulletLifetimeMs
        );
        this->m_MapSystem->AddBullet(bullet);
    }
}

void GhostKing::StartScarletFlame(const std::shared_ptr<Character> &target) {
    this->SetIncorporeal(false);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_ScarletShotsFired = 0;
    this->m_ScarletTotalShots =
        this->m_Phase == Phase::Angry ?
            kScarletAngryShots :
            kScarletNormalShots;
    this->m_NextSkillActionTime = Util::Time::GetElapsedTimeMs();
    this->m_LockedSkillDirection =
        target == nullptr ?
        this->NormalizeOrFallback(this->m_FaceDirection) :
        this->NormalizeOrFallback(
            target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation()
        );
    this->m_FaceDirection = this->m_LockedSkillDirection;
    this->SetAttackAnimation(this->m_AttackAnimation);
    this->TriggerAttackVisual(190.0F);
}

void GhostKing::UpdateScarletFlame() {
    this->m_MoveIntent = {0.0F, 0.0F};

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    while (this->m_ScarletShotsFired < this->m_ScarletTotalShots &&
           now >= this->m_NextSkillActionTime) {
        this->FireScarletBullet();
        this->m_ScarletShotsFired++;
        this->m_NextSkillActionTime += kScarletIntervalMs;
        this->TriggerAttackVisual(150.0F);
    }

    if (this->m_ScarletShotsFired >= this->m_ScarletTotalShots) {
        this->StartCooldownOrRelocate();
    }
}

void GhostKing::FireScarletBullet() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 direction = this->NormalizeOrFallback(
        this->RotateVector(
            this->m_LockedSkillDirection,
            ToRadians(this->RandomFloat(-7.0F, 7.0F))
        )
    );
    const glm::vec2 origin = this->GetFaceFireOrigin() + direction * 18.0F;
    std::shared_ptr<Bullet> bullet =
        std::make_shared<GhostKingBouncingBullet>(
            origin,
            direction * kScarletBulletSpeed,
            kScarletDamage,
            this->GetFaction(),
            kScarletBulletLifetimeMs
        );
    this->m_MapSystem->AddBullet(bullet);
}

void GhostKing::StartSpiritBurst() {
    this->SetIncorporeal(false);
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_BurstRoundsFired = 0;
    this->m_BurstTotalRounds = this->RandomInt(1, 5);
    this->m_NextSkillActionTime = Util::Time::GetElapsedTimeMs();
    this->SetAttackAnimation(this->m_PressureAnimation);
    this->TriggerAttackVisual(240.0F);
}

void GhostKing::UpdateSpiritBurst() {
    this->m_MoveIntent = {0.0F, 0.0F};

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    while (this->m_BurstRoundsFired < this->m_BurstTotalRounds &&
           now >= this->m_NextSkillActionTime) {
        this->FireSpiritBurst();
        this->m_BurstRoundsFired++;
        this->m_NextSkillActionTime += kSpiritBurstIntervalMs;
        this->TriggerAttackVisual(250.0F);
    }

    if (this->m_BurstRoundsFired >= this->m_BurstTotalRounds) {
        this->StartCooldownOrRelocate();
    }
}

void GhostKing::FireSpiritBurst() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const BulletConfig config = BuildBulletConfig(
        kCrossBulletSprite,
        kSpiritBurstScale,
        kSpiritBurstColliderSize,
        5
    );
    const glm::vec2 origin = this->GetBurstOrigin();
    const float angleOffset = this->RandomFloat(0.0F, kTau);

    for (int index = 0; index < kSpiritBurstBulletCount; ++index) {
        const float angle =
            angleOffset +
            kTau * static_cast<float>(index) /
                static_cast<float>(kSpiritBurstBulletCount);
        const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
        std::shared_ptr<Bullet> bullet = std::make_shared<TimedBullet>(
            config,
            origin + direction * 14.0F,
            direction * kSpiritBurstBulletSpeed,
            kSpiritBurstDamage,
            this->GetFaction(),
            kSpiritBurstBulletLifetimeMs
        );
        this->m_MapSystem->AddBullet(bullet);
    }
}

std::shared_ptr<Character> GhostKing::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

glm::vec2 GhostKing::GetFaceFireOrigin() const {
    const glm::vec2 forward = this->NormalizeOrFallback(this->m_FaceDirection);
    return this->GetAbsoluteTranslation() + forward * 18.0F + glm::vec2(0.0F, 13.0F);
}

glm::vec2 GhostKing::GetBurstOrigin() const {
    return this->GetAbsoluteTranslation() + glm::vec2(0.0F, -26.0F);
}

glm::vec2 GhostKing::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        if (glm::length(this->m_FaceDirection) > 0.0001F) {
            return glm::normalize(this->m_FaceDirection);
        }
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

glm::vec2 GhostKing::RotateVector(const glm::vec2 &vector, float radians) const {
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    return {
        vector.x * cosine - vector.y * sine,
        vector.x * sine + vector.y * cosine
    };
}

glm::vec2 GhostKing::FindSafeBossPosition() {
    if (this->m_MapSystem == nullptr) {
        return this->GetAbsoluteTranslation();
    }

    const std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room == nullptr) {
        return this->GetAbsoluteTranslation();
    }

    const glm::vec2 roomCenter = room->GetAbsoluteTranslation();
    const glm::vec2 roomHalfSize = room->GetAreaSize() * 0.5F;
    const glm::vec2 bossHalfSize = this->GetColliderSize() * 0.5F;
    const float left = roomCenter.x - roomHalfSize.x + kRoomSpawnMargin + bossHalfSize.x;
    const float right = roomCenter.x + roomHalfSize.x - kRoomSpawnMargin - bossHalfSize.x;
    const float bottom = roomCenter.y - roomHalfSize.y + kRoomSpawnMargin + bossHalfSize.y;
    const float top = roomCenter.y + roomHalfSize.y - kRoomSpawnMargin - bossHalfSize.y;

    if (left >= right || bottom >= top) {
        return this->GetAbsoluteTranslation();
    }

    for (int attempt = 0; attempt < kMaxSpawnAttempts; ++attempt) {
        const glm::vec2 candidate = {
            this->RandomFloat(left, right),
            this->RandomFloat(bottom, top)
        };

        if (room->IsPlayerInside(candidate) &&
            this->m_MapSystem->CanOccupy(*this, candidate)) {
            return candidate;
        }
    }

    return this->GetAbsoluteTranslation();
}

glm::vec2 GhostKing::FindSafeGhostPosition(const glm::vec2 &fallback) {
    std::shared_ptr<Ghost> probe =
        std::make_shared<Ghost>(this->m_TracePlayerTemp, this->m_CollisionSystemTemp);

    if (this->m_MapSystem == nullptr) {
        return fallback;
    }

    const std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room == nullptr) {
        return fallback;
    }

    const glm::vec2 roomCenter = room->GetAbsoluteTranslation();
    const glm::vec2 roomHalfSize = room->GetAreaSize() * 0.5F;
    const glm::vec2 ghostHalfSize = probe->GetColliderSize() * 0.5F;
    const float left = roomCenter.x - roomHalfSize.x + kRoomSpawnMargin + ghostHalfSize.x;
    const float right = roomCenter.x + roomHalfSize.x - kRoomSpawnMargin - ghostHalfSize.x;
    const float bottom = roomCenter.y - roomHalfSize.y + kRoomSpawnMargin + ghostHalfSize.y;
    const float top = roomCenter.y + roomHalfSize.y - kRoomSpawnMargin - ghostHalfSize.y;

    if (left >= right || bottom >= top) {
        return fallback;
    }

    for (int attempt = 0; attempt < kMaxSpawnAttempts; ++attempt) {
        const float angle = this->RandomFloat(0.0F, kTau);
        const float radius = this->RandomFloat(30.0F, 88.0F);
        glm::vec2 candidate = {
            fallback.x + std::cos(angle) * radius,
            fallback.y + std::sin(angle) * radius
        };
        candidate.x = std::clamp(candidate.x, left, right);
        candidate.y = std::clamp(candidate.y, bottom, top);

        if (room->IsPlayerInside(candidate) &&
            this->m_MapSystem->CanOccupy(*probe, candidate)) {
            return candidate;
        }
    }

    return fallback;
}

void GhostKing::ShowRelocationWarning(bool visible) {
    if (this->m_RelocationWarningVisual == nullptr) {
        return;
    }

    if (visible) {
        this->m_RelocationWarningVisual->SetAbsoluteTranslation(
            this->m_RelocationTarget
        );
    }

    if (visible && !this->m_WarningAttached) {
        this->AddChild(this->m_RelocationWarningVisual);
        this->m_WarningAttached = true;
    } else if (!visible && this->m_WarningAttached) {
        this->RemoveChild(this->m_RelocationWarningVisual);
        this->m_WarningAttached = false;
    }

    this->m_RelocationWarningVisual->SetVisible(visible);
}

void GhostKing::SpawnGhosts(int count) {
    for (int index = 0; index < count; ++index) {
        this->SpawnGhostAt(this->FindSafeGhostPosition(this->GetAbsoluteTranslation()));
    }
}

void GhostKing::SpawnGhostAt(const glm::vec2 &position) {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    std::shared_ptr<Ghost> ghost =
        std::make_shared<Ghost>(this->m_TracePlayerTemp, this->m_CollisionSystemTemp);
    ghost->SetAbsoluteTranslation(position);
    ghost->SetDamageEnabled(true);

    const std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room != nullptr) {
        room->AddMob(ghost);
    }

    this->m_MapSystem->AddMob(ghost);
}

GhostKing::SkillKind GhostKing::RandomSkillKind() {
    static constexpr std::array<SkillKind, 3> kAllSkills = {
        SkillKind::EyeVolley,
        SkillKind::ScarletFlame,
        SkillKind::SpiritBurst
    };

    std::uniform_int_distribution<std::size_t> distribution(
        0,
        kAllSkills.size() - 1
    );
    return kAllSkills[distribution(this->m_RandomEngine)];
}

int GhostKing::RandomInt(int minValue, int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}

float GhostKing::RandomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}
