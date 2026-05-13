#include "Component/Mobs/VitaminCMecha.hpp"

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
#include "Component/IStateful.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

const std::vector<std::string> kNormalSprite = {
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_stand_0.png",
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_stand_1.png",
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_stand_2.png"
};

const std::vector<std::string> kAngrySprite = {
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_0.png",
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_1.png",
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_2.png",
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_angry_stand_3.png"
};

const std::vector<std::string> kDeathSprite = {
    RESOURCE_DIR"/Mob/VitaminCMecha/VitaminCMecha_die.png"
};

const std::vector<std::string> kRoundBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyRoundBullet.png"
};

const std::vector<std::string> kOvalBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyOvalBullet.png"
};

const std::vector<std::string> kExplosionSprites = {
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_0.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_1.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_2.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_3.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_4.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_5.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_6.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_7.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_8.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_9.png",
    RESOURCE_DIR"/Effect/Explosion/NormalExplosion_10.png"
};

const std::string kLaserResource = RESOURCE_DIR"/Bullet/EnemyLaserBullet.png";
const std::string kWarningResource = RESOURCE_DIR"/Effect/WarningSign.png";

constexpr int kMaxHealth = 600;
constexpr float kBossScale = 1.0F / 3.0F;
const glm::vec2 kBossColliderSize = {98.0F * kBossScale, 78.0F * kBossScale};
constexpr float kBossCenterFireForwardOffset = 18.0F * kBossScale;
constexpr float kBossHandFireForwardOffset = 18.0F * kBossScale;
constexpr float kBossHandFireSideOffset = 38.0F * kBossScale;
constexpr float kMoveSpeed = 0.034F;
constexpr float kRetreatDistance = 140.0F;
constexpr float kPreferredDistance = 200.0F;
constexpr float kApproachDistance = 260.0F;
constexpr float kAggroDistance = 540.0F;
constexpr float kStrafeFlipMinMs = 900.0F;
constexpr float kStrafeFlipMaxMs = 1600.0F;
constexpr float kInitialSkillDelayMs = 700.0F;
constexpr float kSkillCooldownMs = 1100.0F;

constexpr float kPi = 3.14159265358979323846F;
constexpr float kTau = kPi * 2.0F;

constexpr float ToRadians(float degrees) {
    return degrees * kPi / 180.0F;
}

constexpr int kSpiralNormalShots = 26;
constexpr int kSpiralAngryShots = 70;
constexpr float kSpiralNormalIntervalMs = 82.0F;
constexpr float kSpiralAngryIntervalMs = 36.0F;
constexpr float kSpiralAngleStep = kTau / 13.0F;
constexpr float kSpiralBulletSpeed = 0.42F;
constexpr float kSpiralBulletLifetimeMs = 3000.0F;
constexpr int kSpiralBulletDamage = 3;
constexpr float kSpiralBulletVisualScale = 0.72F;
const glm::vec2 kSpiralBulletColliderSize = {24.0F, 24.0F};

constexpr float kLaserSetupMs = 360.0F;
constexpr float kLaserSweepNormalMs = 980.0F;
constexpr float kLaserSweepAngryMs = kLaserSweepNormalMs * 0.5F;
constexpr float kLaserFallbackLength = MAP_PIXEL_PER_BLOCK * 38.0F;
constexpr float kLaserDamageIntervalMs = 360.0F;
constexpr float kLaserHitHalfThickness = 12.0F;
constexpr int kLaserDamage = 5;
constexpr float kLaserNormalThicknessScale = 0.28F;
constexpr float kLaserAngryThicknessScale = 0.42F;
constexpr float kLaserHandSeparation = ToRadians(50.0F);
constexpr float kLaserSweepArc = ToRadians(45.0F);

constexpr int kMachineGunDamage = 3;
constexpr float kMachineGunNormalIntervalMs = 150.0F;
constexpr float kMachineGunAngryIntervalMs = 96.0F;
constexpr float kMachineGunNormalBulletSpeed = 0.48F;
constexpr float kMachineGunAngryBulletSpeed = kMachineGunNormalBulletSpeed * 2.0F;
constexpr float kMachineGunBulletLifetimeMs = 2600.0F;
constexpr float kOvalSpinRadiansPerMs = kTau / 500.0F;
constexpr float kOvalBulletVisualScale = 0.9F;
const glm::vec2 kOvalBulletColliderSize = {26.0F, 16.0F};

constexpr float kDisintegrationNormalDelayMs = 560.0F;
constexpr float kDisintegrationAngryDelayMs = 320.0F;
constexpr float kDisintegrationFinishDelayMs = 460.0F;
constexpr int kDisintegrationMainDamage = 5;
constexpr int kDisintegrationLargeDamage = 3;
constexpr int kDisintegrationChildDamage = 2;
constexpr float kDisintegrationMainSpeed = 0.32F;
constexpr float kDisintegrationLargeSpeed = 0.38F;
constexpr float kDisintegrationChildSpeed = 0.54F;
constexpr float kDisintegrationMainLifetimeMs = 2400.0F;
constexpr float kDisintegrationLargeLifetimeMs = 1800.0F;
constexpr float kDisintegrationChildLifetimeMs = 1600.0F;
constexpr float kDisintegrationMainScale = 1.85F;
constexpr float kDisintegrationLargeScale = 1.35F;
constexpr float kDisintegrationChildScale = 0.76F;
const glm::vec2 kDisintegrationMainColliderSize = {44.0F, 44.0F};
const glm::vec2 kDisintegrationLargeColliderSize = {32.0F, 32.0F};
const glm::vec2 kDisintegrationChildColliderSize = {22.0F, 22.0F};

constexpr float kBombNormalIntervalMs = 500.0F;
constexpr float kBombAngryIntervalMs = 340.0F;
constexpr float kBombWarningDelayMs = 600.0F;
constexpr float kBombExplosionDurationMs = 520.0F;
constexpr float kBombSpawnRadius = 76.0F;
constexpr float kBombDamageRadius = 21.0F;
constexpr int kBombDamage = 5;
constexpr float kBombKnockbackStrength = 0.18F;
constexpr float kBombWarningVisualScale = 0.5F;
constexpr float kBombExplosionVisualScale = 0.625F;

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

glm::vec2 NormalizeDirection(const glm::vec2 &direction) {
    if (glm::length(direction) <= 0.0001F) {
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

glm::vec2 RotateVector(const glm::vec2 &vector, float radians) {
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    return {
        vector.x * cosine - vector.y * sine,
        vector.x * sine + vector.y * cosine
    };
}

BulletConfig BuildRoundBulletConfig(
    float visualScale,
    const glm::vec2 &colliderSize,
    int zIndex = 5
) {
    BulletConfig config;
    config.sprites = kRoundBulletSprite;
    config.visualScale = {visualScale, visualScale};
    config.colliderSize = colliderSize;
    config.zIndex = zIndex;
    config.loopAnimation = true;
    config.frameIntervalMs = 20;
    return config;
}

class VitaminCSpinningOvalBullet : public Bullet {
public:
    VitaminCSpinningOvalBullet(
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    ) : Bullet(kOvalBulletSprite, coordinate, momentum, 5, damage, faction),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()),
        m_LifetimeMs(lifetimeMs),
        m_BaseRotation(std::atan2(momentum.y, momentum.x)) {
        this->SetVisualScalePreservingFlip(
            {kOvalBulletVisualScale, kOvalBulletVisualScale}
        );
        this->SetColliderSize(kOvalBulletColliderSize);
    }

    void Update() override {
        const float elapsedMs = Util::Time::GetElapsedTimeMs() - this->m_SpawnTime;
        if (elapsedMs >= this->m_LifetimeMs) {
            this->RequestDestroy();
            return;
        }

        Bullet::Update();
        this->SetAbsoluteRotation(
            this->m_BaseRotation + elapsedMs * kOvalSpinRadiansPerMs
        );
    }

private:
    Util::ms_t m_SpawnTime = 0.0F;
    float m_LifetimeMs = 0.0F;
    float m_BaseRotation = 0.0F;
};

enum class DisintegrationSplitMode {
    Main,
    Large
};

class VitaminCSplittingBullet : public Bullet {
public:
    VitaminCSplittingBullet(
        DisintegrationSplitMode splitMode,
        MapSystem* mapSystem,
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        int damage,
        CombatFaction faction,
        float visualScale,
        const glm::vec2 &colliderSize,
        float lifetimeMs
    ) : Bullet(kRoundBulletSprite, coordinate, momentum, 5, damage, faction),
        m_SplitMode(splitMode),
        m_MapSystem(mapSystem),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()),
        m_LifetimeMs(lifetimeMs) {
        this->SetVisualScalePreservingFlip({visualScale, visualScale});
        this->SetColliderSize(colliderSize);
    }

    void Update() override {
        const float elapsedMs = Util::Time::GetElapsedTimeMs() - this->m_SpawnTime;
        if (elapsedMs >= this->m_LifetimeMs) {
            this->RequestDestroy();
            this->TriggerSplit();
            return;
        }

        Bullet::Update();

        if (this->IsDestroyRequested()) {
            this->TriggerSplit();
        }
    }

    void OnCollision(const Collision::CollisionSituation &situation) override {
        Bullet::OnCollision(situation);

        if (this->IsDestroyRequested()) {
            this->TriggerSplit();
        }
    }

private:
    void TriggerSplit() {
        if (this->m_SplitTriggered || this->m_MapSystem == nullptr) {
            return;
        }

        this->m_SplitTriggered = true;

        if (this->m_SplitMode == DisintegrationSplitMode::Main) {
            this->SpawnLargeRoundBullets();
            return;
        }

        this->SpawnChildRoundBullets();
    }

    void SpawnLargeRoundBullets() {
        constexpr int kSplitCount = 6;
        const glm::vec2 origin = this->GetAbsoluteTranslation();

        for (int index = 0; index < kSplitCount; ++index) {
            const float angle =
                kTau * static_cast<float>(index) / static_cast<float>(kSplitCount);
            const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
            std::shared_ptr<Bullet> bullet =
                std::make_shared<VitaminCSplittingBullet>(
                    DisintegrationSplitMode::Large,
                    this->m_MapSystem,
                    origin + direction * 12.0F,
                    direction * kDisintegrationLargeSpeed,
                    kDisintegrationLargeDamage,
                    this->GetFaction(),
                    kDisintegrationLargeScale,
                    kDisintegrationLargeColliderSize,
                    kDisintegrationLargeLifetimeMs
                );
            this->m_MapSystem->AddBullet(bullet);
        }
    }

    void SpawnChildRoundBullets() {
        constexpr int kSplitCount = 12;
        const glm::vec2 origin = this->GetAbsoluteTranslation();
        const float baseAngle = std::atan2(this->GetMomentum().y, this->GetMomentum().x);
        const BulletConfig config = BuildRoundBulletConfig(
            kDisintegrationChildScale,
            kDisintegrationChildColliderSize
        );

        for (int index = 0; index < kSplitCount; ++index) {
            const float angle =
                baseAngle +
                kTau * static_cast<float>(index) / static_cast<float>(kSplitCount);
            const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
            std::shared_ptr<Bullet> bullet = std::make_shared<TimedBullet>(
                config,
                origin + direction * 10.0F,
                direction * kDisintegrationChildSpeed,
                kDisintegrationChildDamage,
                this->GetFaction(),
                kDisintegrationChildLifetimeMs
            );
            this->m_MapSystem->AddBullet(bullet);
        }
    }

    DisintegrationSplitMode m_SplitMode = DisintegrationSplitMode::Main;
    MapSystem* m_MapSystem = nullptr;
    Util::ms_t m_SpawnTime = 0.0F;
    float m_LifetimeMs = 0.0F;
    bool m_SplitTriggered = false;
};

} // namespace

class VitaminCBombEffect : public Util::GameObject,
                           public MapObject,
                           public IStateful {
public:
    VitaminCBombEffect(MapSystem* mapSystem, const glm::vec2 &position)
        : Util::GameObject(
              std::make_shared<Util::Image>(kWarningResource, false),
              6.2F
          ),
          m_MapSystem(mapSystem),
          m_SpawnTime(Util::Time::GetElapsedTimeMs()),
          m_ExplodeTime(this->m_SpawnTime + kBombWarningDelayMs),
          m_FinishTime(
              this->m_SpawnTime + kBombWarningDelayMs + kBombExplosionDurationMs
          ),
          m_ExplosionAnimation(std::make_shared<Util::Animation>(
              kExplosionSprites,
              false,
              45,
              false,
              0,
              false
          )) {
        this->SetAbsoluteTranslation(position);
        this->SetAbsoluteScale(
            {kBombWarningVisualScale, kBombWarningVisualScale}
        );
    }

    Util::Transform GetObjectTransform() const override {
        return this->m_Transform;
    }

    void Update() override {
        if (this->m_Finished) {
            return;
        }

        const Util::ms_t now = Util::Time::GetElapsedTimeMs();
        if (!this->m_Exploded && now >= this->m_ExplodeTime) {
            this->Explode();
        }

        if (this->m_Exploded && now >= this->m_FinishTime) {
            this->SetVisible(false);
            this->SetDrawable(nullptr);
            this->m_Finished = true;
        }
    }

    bool IsFinished() const {
        return this->m_Finished;
    }

private:
    void Explode() {
        this->m_Exploded = true;
        this->ApplyExplosionDamage();

        if (this->m_ExplosionAnimation != nullptr) {
            this->m_ExplosionAnimation->SetCurrentFrame(0);
            this->m_ExplosionAnimation->Play();
            this->SetDrawable(this->m_ExplosionAnimation);
        }

        this->SetZIndex(6.4F);
        this->SetAbsoluteScale(
            {kBombExplosionVisualScale, kBombExplosionVisualScale}
        );
    }

    void ApplyExplosionDamage() {
        if (this->m_MapSystem == nullptr || this->m_DamageApplied) {
            return;
        }

        this->m_DamageApplied = true;
        const glm::vec2 center = this->GetAbsoluteTranslation();

        for (const auto &player : this->m_MapSystem->GetPlayers()) {
            if (player == nullptr || player->IsDead()) {
                continue;
            }

            const float playerRadius =
                std::max(player->GetColliderSize().x, player->GetColliderSize().y) *
                0.5F;
            if (glm::distance(player->GetAbsoluteTranslation(), center) <=
                kBombDamageRadius + playerRadius) {
                player->ApplyDamage(kBombDamage);
                glm::vec2 knockbackDirection =
                    player->GetAbsoluteTranslation() - center;
                if (glm::length(knockbackDirection) <= 0.0001F) {
                    knockbackDirection = {1.0F, 0.0F};
                }
                player->ApplyImpulse(
                    glm::normalize(knockbackDirection) * kBombKnockbackStrength
                );
            }
        }
    }

    MapSystem* m_MapSystem = nullptr;
    Util::ms_t m_SpawnTime = 0.0F;
    Util::ms_t m_ExplodeTime = 0.0F;
    Util::ms_t m_FinishTime = 0.0F;
    std::shared_ptr<Util::Animation> m_ExplosionAnimation;
    bool m_Exploded = false;
    bool m_DamageApplied = false;
    bool m_Finished = false;
};

VitaminCMechaBeamVisual::VitaminCMechaBeamVisual(
    const std::string &resource,
    float zIndex
) : Util::GameObject(std::make_shared<Util::Image>(resource), zIndex) {
}

Util::Transform VitaminCMechaBeamVisual::GetObjectTransform() const {
    return this->m_Transform;
}

void VitaminCMechaBeamVisual::ConfigureBeam(
    const glm::vec2 &start,
    const glm::vec2 &direction,
    float length,
    float thicknessScale
) {
    const glm::vec2 forward = NormalizeDirection(direction);
    const float safeLength = std::max(1.0F, length);

    this->SetAbsoluteTranslation(start + forward * (safeLength * 0.5F));
    this->SetAbsoluteRotation(std::atan2(forward.y, forward.x));
    this->SetAbsoluteScale({safeLength / 48.0F, thicknessScale});
}

VitaminCMecha::VitaminCMecha(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kNormalSprite,
        kNormalSprite,
        kDeathSprite,
        tracePlayer,
        collisionSystem
    ),
    m_RandomEngine(std::random_device{}()) {
    this->m_NormalAnimation = this->m_StandAnimation;
    this->m_AngryAnimation = std::make_shared<Util::Animation>(
        kAngrySprite,
        true,
        20,
        true,
        0,
        false
    );

    this->SetMaxHealth(kMaxHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetAbsoluteScale({kBossScale, kBossScale});
    this->SetColliderSize(kBossColliderSize);
    this->SetWeapon(nullptr);
    this->m_PlayerSpeed = kMoveSpeed;

    this->m_LeftLaserVisual =
        std::make_shared<VitaminCMechaBeamVisual>(kLaserResource, 6.0F);
    this->m_RightLaserVisual =
        std::make_shared<VitaminCMechaBeamVisual>(kLaserResource, 6.0F);
    this->m_LeftLaserVisual->SetVisible(false);
    this->m_RightLaserVisual->SetVisible(false);

    this->m_NextSkill = this->RandomSkillKind();
    this->m_NextSkillTime = Util::Time::GetElapsedTimeMs() + kInitialSkillDelayMs;
}

void VitaminCMecha::Update() {
    this->PruneBombEffects();

    if (this->IsDead()) {
        this->HideLaserBeams();
        Character::Update();
        return;
    }

    this->UpdatePhase();

    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        this->HideLaserBeams();
        Character::Update();
        return;
    }

    this->UpdateState();
    Character::Update();
}

void VitaminCMecha::ApplyDamage(int damage) {
    Mob::ApplyDamage(damage);
    this->UpdatePhase();
}

glm::vec2 VitaminCMecha::GetMoveIntent() const {
    return this->m_MoveIntent;
}

glm::vec2 VitaminCMecha::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void VitaminCMecha::UpdateWeaponPresentation() {
}

void VitaminCMecha::UpdatePhase() {
    if (this->m_Phase == Phase::Angry || this->IsDead()) {
        return;
    }

    if (this->GetCurrentHealth() * 2 <= this->GetMaxHealth()) {
        this->EnterAngryPhase();
    }
}

void VitaminCMecha::EnterAngryPhase() {
    this->m_Phase = Phase::Angry;
    this->m_StandAnimation = this->m_AngryAnimation;
    this->m_WalkAnimation = this->m_AngryAnimation;
    this->SetDrawable(this->m_AngryAnimation);
}

void VitaminCMecha::UpdateState() {
    const std::shared_ptr<Character> target = this->GetTarget();
    if (target == nullptr || target->IsDead()) {
        this->m_MoveIntent = {0.0F, 0.0F};
        this->HideLaserBeams();
        return;
    }

    const glm::vec2 toTarget =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    if (glm::length(toTarget) > 0.0001F) {
        this->m_FaceDirection = glm::normalize(toTarget);
    }

    switch (this->m_BossState) {
    case BossState::Idle:
        this->UpdateSlowMovement(target);
        if (glm::length(toTarget) <= kAggroDistance &&
            Util::Time::GetElapsedTimeMs() >= this->m_NextSkillTime) {
            switch (this->m_NextSkill) {
            case SkillKind::SpiralCannon:
                this->EnterState(BossState::SpiralCannon);
                break;
            case SkillKind::LaserCannon:
                this->EnterState(BossState::LaserCannon);
                break;
            case SkillKind::MachineGun:
                this->EnterState(BossState::MachineGun);
                break;
            case SkillKind::DisintegrationCannon:
                this->EnterState(BossState::DisintegrationCannon);
                break;
            case SkillKind::Bombardment:
                this->EnterState(BossState::Bombardment);
                break;
            }
        }
        break;

    case BossState::Cooldown:
        this->UpdateSlowMovement(target);
        if (Util::Time::GetElapsedTimeMs() >= this->m_NextSkillTime) {
            this->EnterState(BossState::Idle);
        }
        break;

    case BossState::SpiralCannon:
        this->UpdateSpiralCannon();
        break;

    case BossState::LaserCannon:
        this->UpdateLaserCannon();
        break;

    case BossState::MachineGun:
        this->UpdateMachineGun(target);
        break;

    case BossState::DisintegrationCannon:
        this->UpdateDisintegrationCannon(target);
        break;

    case BossState::Bombardment:
        this->UpdateBombardment(target);
        break;
    }
}

void VitaminCMecha::EnterState(BossState state) {
    this->m_BossState = state;
    this->m_StateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_MoveIntent = {0.0F, 0.0F};

    this->HideLaserBeams();

    switch (state) {
    case BossState::Idle:
        break;

    case BossState::Cooldown:
        this->m_NextSkill = this->RandomSkillKind();
        this->m_NextSkillTime =
            Util::Time::GetElapsedTimeMs() + kSkillCooldownMs;
        break;

    case BossState::SpiralCannon:
        this->StartSpiralCannon();
        break;

    case BossState::LaserCannon:
        this->StartLaserCannon();
        break;

    case BossState::MachineGun:
        this->StartMachineGun();
        break;

    case BossState::DisintegrationCannon:
        this->StartDisintegrationCannon();
        break;

    case BossState::Bombardment:
        this->StartBombardment();
        break;
    }
}

void VitaminCMecha::UpdateSlowMovement(const std::shared_ptr<Character> &target) {
    if (target == nullptr) {
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

    if (distance > kApproachDistance) {
        this->m_MoveIntent = forward;
        return;
    }

    if (distance < kRetreatDistance) {
        this->m_MoveIntent = -forward;
        return;
    }

    const float holdBias =
        distance < kPreferredDistance ? -0.28F :
        distance > kPreferredDistance ? 0.24F :
        0.0F;
    this->m_MoveIntent = this->NormalizeOrFallback(
        side * static_cast<float>(this->m_StrafeDirection) + forward * holdBias
    );
}

void VitaminCMecha::PruneBombEffects() {
    auto removeBegin = std::remove_if(
        this->m_BombEffects.begin(),
        this->m_BombEffects.end(),
        [this](const std::shared_ptr<VitaminCBombEffect> &effect) {
            if (effect == nullptr || !effect->IsFinished()) {
                return false;
            }

            this->RemoveChild(effect);
            return true;
        }
    );

    this->m_BombEffects.erase(removeBegin, this->m_BombEffects.end());
}

void VitaminCMecha::StartSpiralCannon() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_SpiralShotsFired = 0;
    this->m_SpiralTotalShots =
        this->m_Phase == Phase::Angry ? kSpiralAngryShots : kSpiralNormalShots;
    this->m_SpiralAngle = this->RandomFloat(0.0F, kTau);
    this->m_NextSpiralShotTime = Util::Time::GetElapsedTimeMs();
}

void VitaminCMecha::UpdateSpiralCannon() {
    this->m_MoveIntent = {0.0F, 0.0F};
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const float interval =
        this->m_Phase == Phase::Angry ?
            kSpiralAngryIntervalMs :
            kSpiralNormalIntervalMs;

    while (this->m_SpiralShotsFired < this->m_SpiralTotalShots &&
           now >= this->m_NextSpiralShotTime) {
        this->FireOneSpiralBullet();
        this->m_SpiralShotsFired++;
        this->m_NextSpiralShotTime += interval;
    }

    if (this->m_SpiralShotsFired >= this->m_SpiralTotalShots) {
        this->EnterState(BossState::Cooldown);
    }
}

void VitaminCMecha::FireOneSpiralBullet() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 direction = {
        std::cos(this->m_SpiralAngle),
        std::sin(this->m_SpiralAngle)
    };
    const BulletConfig config = BuildRoundBulletConfig(
        kSpiralBulletVisualScale,
        kSpiralBulletColliderSize
    );
    std::shared_ptr<Bullet> bullet = std::make_shared<TimedBullet>(
        config,
        this->GetCenterFireOrigin() + direction * 12.0F,
        direction * kSpiralBulletSpeed,
        kSpiralBulletDamage,
        this->GetFaction(),
        kSpiralBulletLifetimeMs
    );
    this->m_MapSystem->AddBullet(bullet);
    this->m_SpiralAngle += kSpiralAngleStep;
}

void VitaminCMecha::StartLaserCannon() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_NextLaserDamageTime =
        Util::Time::GetElapsedTimeMs() + kLaserSetupMs;
}

void VitaminCMecha::UpdateLaserCannon() {
    const std::shared_ptr<Character> target = this->GetTarget();
    if (target == nullptr || target->IsDead()) {
        this->EnterState(BossState::Cooldown);
        return;
    }

    this->m_MoveIntent = {0.0F, 0.0F};
    const glm::vec2 baseDirection = this->NormalizeOrFallback(
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation()
    );
    this->m_FaceDirection = baseDirection;

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const float elapsedMs = now - this->m_StateStartTime;
    if (elapsedMs < kLaserSetupMs) {
        return;
    }

    const float sweepMs =
        this->m_Phase == Phase::Angry ?
            kLaserSweepAngryMs :
            kLaserSweepNormalMs;
    const float activeMs = elapsedMs - kLaserSetupMs;

    if (activeMs >= sweepMs * 2.0F) {
        this->HideLaserBeams();
        this->EnterState(BossState::Cooldown);
        return;
    }

    const bool firstPass = activeMs < sweepMs;
    const float passElapsed = firstPass ? activeMs : activeMs - sweepMs;
    const float progress = std::clamp(passElapsed / sweepMs, 0.0F, 1.0F);
    const float turnSign = this->m_FaceDirection.x >= 0.0F ? -1.0F : 1.0F;
    const float handOffset =
        (firstPass ? -kLaserHandSeparation : kLaserHandSeparation) * 0.5F;
    const float startOffset = 0.0F;
    const float endOffset = firstPass ? kLaserSweepArc : -kLaserSweepArc;
    const float sweepOffset =
        startOffset + (endOffset - startOffset) * progress;
    const glm::vec2 laserDirection =
        this->NormalizeOrFallback(
            RotateVector(baseDirection, (handOffset + sweepOffset) * turnSign)
        );

    const glm::vec2 start = this->GetCenterFireOrigin();
    const float thicknessScale =
        this->m_Phase == Phase::Angry ?
            kLaserAngryThicknessScale :
            kLaserNormalThicknessScale;
    const float laserLength =
        this->ComputeRoomClippedBeamLength(
            start,
            laserDirection,
            kLaserFallbackLength
        );

    const std::shared_ptr<VitaminCMechaBeamVisual> activeBeam =
        firstPass ? this->m_LeftLaserVisual : this->m_RightLaserVisual;
    const std::shared_ptr<VitaminCMechaBeamVisual> inactiveBeam =
        firstPass ? this->m_RightLaserVisual : this->m_LeftLaserVisual;

    if (firstPass) {
        this->SetLaserBeamVisible(
            inactiveBeam,
            this->m_RightLaserAttached,
            false
        );
    } else {
        this->SetLaserBeamVisible(
            inactiveBeam,
            this->m_LeftLaserAttached,
            false
        );
    }
    this->ConfigureLaserBeam(activeBeam, start, laserDirection, thicknessScale);

    if (now >= this->m_NextLaserDamageTime) {
        this->DamagePlayersAlongLaser(
            start,
            start + laserDirection * laserLength
        );
        this->m_NextLaserDamageTime = now + kLaserDamageIntervalMs;
    }
}

void VitaminCMecha::ConfigureLaserBeam(
    const std::shared_ptr<VitaminCMechaBeamVisual> &beam,
    const glm::vec2 &start,
    const glm::vec2 &direction,
    float thicknessScale
) {
    if (beam == nullptr) {
        return;
    }

    const float length =
        this->ComputeRoomClippedBeamLength(start, direction, kLaserFallbackLength);
    beam->ConfigureBeam(start, direction, length, thicknessScale);

    if (beam == this->m_LeftLaserVisual) {
        this->SetLaserBeamVisible(beam, this->m_LeftLaserAttached, true);
        return;
    }

    if (beam == this->m_RightLaserVisual) {
        this->SetLaserBeamVisible(beam, this->m_RightLaserAttached, true);
    }
}

void VitaminCMecha::SetLaserBeamVisible(
    const std::shared_ptr<VitaminCMechaBeamVisual> &beam,
    bool &attached,
    bool visible
) {
    if (beam == nullptr) {
        return;
    }

    if (visible && !attached) {
        this->AddChild(beam);
        attached = true;
    } else if (!visible && attached) {
        this->RemoveChild(beam);
        attached = false;
    }

    beam->SetVisible(visible);
}

void VitaminCMecha::HideLaserBeams() {
    this->SetLaserBeamVisible(
        this->m_LeftLaserVisual,
        this->m_LeftLaserAttached,
        false
    );
    this->SetLaserBeamVisible(
        this->m_RightLaserVisual,
        this->m_RightLaserAttached,
        false
    );
}

void VitaminCMecha::DamagePlayersAlongLaser(
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
            std::max(player->GetColliderSize().x, player->GetColliderSize().y) *
            0.5F;
        if (DistancePointToSegment(player->GetAbsoluteTranslation(), start, end) <=
            playerRadius + kLaserHitHalfThickness) {
            player->ApplyDamage(kLaserDamage);
        }
    }
}

void VitaminCMecha::StartMachineGun() {
    const int rounds = this->RandomInt(2, 4);
    this->m_MachineGunShotsFired = 0;
    this->m_MachineGunTotalShots = rounds * 2;
    this->m_NextMachineGunShotTime = Util::Time::GetElapsedTimeMs();
}

void VitaminCMecha::UpdateMachineGun(const std::shared_ptr<Character> &target) {
    this->UpdateSlowMovement(target);

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const float interval =
        this->m_Phase == Phase::Angry ?
            kMachineGunAngryIntervalMs :
            kMachineGunNormalIntervalMs;

    while (this->m_MachineGunShotsFired < this->m_MachineGunTotalShots &&
           now >= this->m_NextMachineGunShotTime) {
        this->FireMachineGunBullet(target);
        this->m_MachineGunShotsFired++;
        this->m_NextMachineGunShotTime += interval;
    }

    if (this->m_MachineGunShotsFired >= this->m_MachineGunTotalShots) {
        this->EnterState(BossState::Cooldown);
    }
}

void VitaminCMecha::FireMachineGunBullet(
    const std::shared_ptr<Character> &target
) {
    if (this->m_MapSystem == nullptr || target == nullptr) {
        return;
    }

    const int handIndex = this->m_MachineGunShotsFired % 2;
    const glm::vec2 origin = this->GetHandFireOrigin(handIndex);
    const glm::vec2 direction = this->NormalizeOrFallback(
        target->GetAbsoluteTranslation() - origin
    );
    const float speed =
        this->m_Phase == Phase::Angry ?
            kMachineGunAngryBulletSpeed :
            kMachineGunNormalBulletSpeed;

    std::shared_ptr<Bullet> bullet = std::make_shared<VitaminCSpinningOvalBullet>(
        origin + direction * 18.0F,
        direction * speed,
        kMachineGunDamage,
        this->GetFaction(),
        kMachineGunBulletLifetimeMs
    );
    this->m_MapSystem->AddBullet(bullet);
}

void VitaminCMecha::StartDisintegrationCannon() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_DisintegrationFired = false;
    this->m_DisintegrationFireTime =
        Util::Time::GetElapsedTimeMs() +
        (this->m_Phase == Phase::Angry ?
            kDisintegrationAngryDelayMs :
            kDisintegrationNormalDelayMs);
}

void VitaminCMecha::UpdateDisintegrationCannon(
    const std::shared_ptr<Character> &target
) {
    this->m_MoveIntent = {0.0F, 0.0F};

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (!this->m_DisintegrationFired &&
        now >= this->m_DisintegrationFireTime) {
        this->FireDisintegrationBullet(target);
        this->m_DisintegrationFired = true;
    }

    if (this->m_DisintegrationFired &&
        now >= this->m_DisintegrationFireTime + kDisintegrationFinishDelayMs) {
        this->EnterState(BossState::Cooldown);
    }
}

void VitaminCMecha::FireDisintegrationBullet(
    const std::shared_ptr<Character> &target
) {
    if (this->m_MapSystem == nullptr || target == nullptr) {
        return;
    }

    const glm::vec2 origin = this->GetCenterFireOrigin();
    const glm::vec2 direction = this->NormalizeOrFallback(
        target->GetAbsoluteTranslation() - origin
    );

    std::shared_ptr<Bullet> bullet =
        std::make_shared<VitaminCSplittingBullet>(
            DisintegrationSplitMode::Main,
            this->m_MapSystem,
            origin + direction * 18.0F,
            direction * kDisintegrationMainSpeed,
            kDisintegrationMainDamage,
            this->GetFaction(),
            kDisintegrationMainScale,
            kDisintegrationMainColliderSize,
            kDisintegrationMainLifetimeMs
        );
    this->m_MapSystem->AddBullet(bullet);
}

void VitaminCMecha::StartBombardment() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_TotalBombs = this->RandomInt(4, 10);
    this->m_BombsSpawned = 0;
    this->m_NextBombTime = Util::Time::GetElapsedTimeMs();
}

void VitaminCMecha::UpdateBombardment(
    const std::shared_ptr<Character> &target
) {
    this->m_MoveIntent = {0.0F, 0.0F};

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const float interval =
        this->m_Phase == Phase::Angry ?
            kBombAngryIntervalMs :
            kBombNormalIntervalMs;

    while (this->m_BombsSpawned < this->m_TotalBombs &&
           now >= this->m_NextBombTime) {
        this->SpawnBombWarning(target);
        this->m_BombsSpawned++;
        this->m_NextBombTime += interval;
    }

    const float minimumDuration =
        static_cast<float>(this->m_TotalBombs) * interval +
        kBombWarningDelayMs +
        kBombExplosionDurationMs;
    if (this->m_BombsSpawned >= this->m_TotalBombs &&
        now - this->m_StateStartTime >= minimumDuration) {
        this->EnterState(BossState::Cooldown);
    }
}

void VitaminCMecha::SpawnBombWarning(
    const std::shared_ptr<Character> &target
) {
    if (target == nullptr) {
        return;
    }

    const float angle = this->RandomFloat(0.0F, kTau);
    const float radius = this->RandomFloat(0.0F, kBombSpawnRadius);
    const glm::vec2 offset = {std::cos(angle) * radius, std::sin(angle) * radius};
    const glm::vec2 position = target->GetAbsoluteTranslation() + offset;

    std::shared_ptr<VitaminCBombEffect> effect =
        std::make_shared<VitaminCBombEffect>(this->m_MapSystem, position);
    this->m_BombEffects.push_back(effect);
    this->AddChild(effect);
}

glm::vec2 VitaminCMecha::GetCenterFireOrigin() const {
    const glm::vec2 forward = this->NormalizeOrFallback(this->m_FaceDirection);
    return this->GetAbsoluteTranslation() + forward * kBossCenterFireForwardOffset;
}

glm::vec2 VitaminCMecha::GetHandFireOrigin(int handIndex) const {
    const glm::vec2 forward = this->NormalizeOrFallback(this->m_FaceDirection);
    const glm::vec2 side = {-forward.y, forward.x};
    const float sideSign = handIndex == 0 ? -1.0F : 1.0F;

    return this->GetAbsoluteTranslation() +
        forward * kBossHandFireForwardOffset +
        side * (sideSign * kBossHandFireSideOffset);
}

float VitaminCMecha::ComputeRoomClippedBeamLength(
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

std::shared_ptr<Character> VitaminCMecha::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

glm::vec2 VitaminCMecha::NormalizeOrFallback(
    const glm::vec2 &direction
) const {
    if (glm::length(direction) <= 0.0001F) {
        if (glm::length(this->m_FaceDirection) > 0.0001F) {
            return glm::normalize(this->m_FaceDirection);
        }
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

VitaminCMecha::SkillKind VitaminCMecha::RandomSkillKind() {
    static constexpr std::array<SkillKind, 5> kAllSkills = {
        SkillKind::SpiralCannon,
        SkillKind::LaserCannon,
        SkillKind::MachineGun,
        SkillKind::DisintegrationCannon,
        SkillKind::Bombardment
    };

    std::uniform_int_distribution<std::size_t> distribution(
        0,
        kAllSkills.size() - 1
    );
    return kAllSkills[distribution(this->m_RandomEngine)];
}

int VitaminCMecha::RandomInt(int minValue, int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}

float VitaminCMecha::RandomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(this->m_RandomEngine);
}
