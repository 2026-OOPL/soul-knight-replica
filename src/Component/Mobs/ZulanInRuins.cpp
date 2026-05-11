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

constexpr std::size_t kCannonCount = 5;

const std::array<const char *, kCannonCount> kCannonSprites = {
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponTopLeft.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponTopRight.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponBottomLeft.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponBottomRight.png",
    RESOURCE_DIR"/Mob/Zulan in Ruins/ZulanInRuinsWeaponTopRight.png"
};

const std::array<glm::vec2, kCannonCount> kCannonHomeOffsets = {
    glm::vec2{-26.0F, 14.0F},
    glm::vec2{26.0F, 14.0F},
    glm::vec2{-22.0F, -10.0F},
    glm::vec2{22.0F, -10.0F},
    glm::vec2{0.0F, 24.0F}
};

const std::vector<std::string> kBarBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyBarBullet.png"
};

const std::vector<std::string> kHeavyArtilleryBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyRoundBullet.png"
};

const std::vector<std::string> kLightArtilleryBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyMagicBullet.png"
};

const std::vector<std::string> kBombTrapConeBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyConeBullet.png"
};

const std::vector<std::string> kBombTrapBubbleBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyBubbleBullet.png"
};

const std::vector<std::string> kPowerfulMotherBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyLightOrbBullet.png"
};

const std::vector<std::string> kPowerfulChildBulletSprite = {
    RESOURCE_DIR"/Bullet/EnemyRoundBullet.png"
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
constexpr float kSkillCooldownMs = 1300.0F;
constexpr bool kDebugForceInterceptLaserOnly = false;
constexpr bool kDebugForceArtilleryOnly = false;
constexpr bool kDebugForceBombTrapOnly = true;
constexpr bool kDebugForceFloatingSatelliteOnly = false;
constexpr bool kDebugForcePowerfulMotherBulletOnly = false;

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
constexpr float kInterceptLineForwardOffset = 96.0F;
constexpr float kInterceptLineSpacing = 36.0F;
constexpr float kInterceptFormationGraceMs = 760.0F;
constexpr int kInterceptSecondRoundFrontCount = 2;

constexpr int kAutoChaseDamage = 2;
constexpr int kHeavyArtilleryDamage = 3;
constexpr int kLightArtilleryDamage = 2;
constexpr int kBombTrapConeDamage = 3;
constexpr int kBombTrapBubbleDamage = 2;
constexpr int kFloatingSatelliteOrbDamage = 1;
constexpr int kFloatingSatelliteBarDamage = 2;
constexpr int kPowerfulMotherBulletDamage = 3;
constexpr int kPowerfulChildBulletDamage = 3;
constexpr int kBarBurstCount = 5;
constexpr int kHeavyArtilleryBulletCount = 9;
constexpr int kBombTrapConeBulletCount = 9;
constexpr int kPowerfulChildBulletCount = 4;
constexpr float kBarBulletSpeed = 0.54F;
constexpr float kBarBulletSpreadRadians = 0.36F;
constexpr float kBarBulletLifetimeMs = 2600.0F;
constexpr float kHeavyArtilleryBulletSpeed = 0.44F;
constexpr float kHeavyArtilleryBulletLifetimeMs = 3200.0F;
constexpr float kLightArtilleryBulletSpeed = 0.66F;
constexpr float kLightArtilleryBulletLifetimeMs = 2400.0F;
constexpr float kLightArtilleryDurationMs = 2300.0F;
constexpr float kLightArtilleryShotIntervalMs = 145.0F;
constexpr float kChestFireForwardOffset = 22.0F;
constexpr float kBombTrapConeFanRadians = 2.1F;
constexpr float kBombTrapConeBulletSpeed = 0.48F;
constexpr float kBombTrapConeBulletLifetimeMs = 3000.0F;
constexpr float kBombTrapConeEmitInitialDistance = 42.0F;
constexpr float kBombTrapConeEmitIntervalDistance = 58.0F;
constexpr float kBombTrapBubbleInitialSpeed = 0.54F;
constexpr float kBombTrapBubbleFinalSpeed = 0.08F;
constexpr float kBombTrapBubbleLifetimeMs = 3800.0F;
constexpr float kBombTrapBubbleSpawnOffset = 14.0F;
constexpr float kFloatingSatelliteLifetimeMs = 4200.0F;
constexpr float kFloatingSatelliteCannonDriftMs = 360.0F;
constexpr float kFloatingSatelliteRotationSpeedRadiansPerMs = 0.002992F;
constexpr float kFloatingSatelliteStartDistance = 40.0F;
constexpr float kFloatingSatelliteSpacing = 48.0F;
constexpr int kFloatingSatelliteOrbZIndex = 6;
constexpr float kFloatingSatelliteOrbContactDamageIntervalMs = 450.0F;
constexpr float kFloatingSatelliteBarBulletSpeed = 0.28F;
constexpr float kFloatingSatelliteBarBulletLifetimeMs = 2600.0F;
constexpr float kFloatingSatelliteBarEmitIntervalMs = 420.0F;
constexpr float kFloatingSatelliteBarEmitInitialDelayMs = 150.0F;
constexpr float kFloatingSatelliteBarSpawnOffset = 18.0F;
constexpr float kPowerfulMotherBulletSpeed = 0.36F;
constexpr float kPowerfulMotherBulletLifetimeMs = 3300.0F;
constexpr float kPowerfulChildBulletSpeed = 0.58F;
constexpr float kPowerfulChildBulletLifetimeMs = 1800.0F;
constexpr float kPowerfulChildBulletEmitIntervalMs = 150.0F;
constexpr float kPowerfulChildBulletEmitInitialDelayMs = 90.0F;
constexpr float kPowerfulChildBulletVortexRadiansPerMs = 0.0062F;
constexpr float kPowerfulChildBulletSpawnOffset = 22.0F;
constexpr float kZulanBarBulletVisualScale = 1.0F;
const glm::vec2 kZulanBarBulletColliderSize = {16.0F, 8.0F};
constexpr float kZulanRoundBulletVisualScale = 0.68F;
const glm::vec2 kZulanRoundBulletColliderSize = {26.0F, 26.0F};
constexpr float kZulanMagicBulletVisualScale = 0.45F;
const glm::vec2 kZulanMagicBulletColliderSize = {17.0F, 17.0F};
constexpr float kZulanConeBulletVisualScale = 0.75F;
const glm::vec2 kZulanConeBulletColliderSize = {26.0F, 19.0F};
constexpr float kZulanBubbleBulletVisualScale = 0.85F;
const glm::vec2 kZulanBubbleBulletColliderSize = {26.0F, 26.0F};
constexpr float kZulanLightOrbBulletVisualScale = 0.55F;
const glm::vec2 kZulanLightOrbBulletColliderSize = {23.0F, 23.0F};
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

void ConfigureBulletSize(
    Bullet &bullet,
    float visualScale,
    const glm::vec2 &colliderSize
) {
    const glm::vec2 currentScale = bullet.GetAbsoluteScale();
    const glm::vec2 scaleSign = {
        currentScale.x < 0.0F ? -1.0F : 1.0F,
        currentScale.y < 0.0F ? -1.0F : 1.0F
    };
    bullet.SetAbsoluteScale(scaleSign * visualScale);
    bullet.SetColliderSize(colliderSize);
}

void ConfigureBulletSize(
    const std::shared_ptr<Bullet> &bullet,
    float visualScale,
    const glm::vec2 &colliderSize
) {
    if (bullet == nullptr) {
        return;
    }

    ConfigureBulletSize(*bullet, visualScale, colliderSize);
}

float GetFloatingSatelliteAngle(
    float startAngle,
    int rotationDirection,
    float elapsedMs
) {
    return startAngle +
        static_cast<float>(rotationDirection) *
            kFloatingSatelliteRotationSpeedRadiansPerMs *
            elapsedMs;
}

glm::vec2 GetFloatingSatelliteDirection(
    float startAngle,
    int rotationDirection,
    float elapsedMs
) {
    const float angle =
        GetFloatingSatelliteAngle(startAngle, rotationDirection, elapsedMs);
    return {std::cos(angle), std::sin(angle)};
}

glm::vec2 GetFloatingSatellitePosition(
    const glm::vec2 &center,
    std::size_t index,
    float startAngle,
    int rotationDirection,
    float elapsedMs
) {
    const glm::vec2 direction =
        GetFloatingSatelliteDirection(startAngle, rotationDirection, elapsedMs);
    const float distance =
        kFloatingSatelliteStartDistance +
            kFloatingSatelliteSpacing * static_cast<float>(index);
    return center + direction * distance;
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

class ZulanDeceleratingBullet : public Bullet {
public:
    ZulanDeceleratingBullet(
        const std::vector<std::string> &sprite,
        const glm::vec2 &coordinate,
        const glm::vec2 &direction,
        float initialSpeed,
        float finalSpeed,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    ) : Bullet(
            sprite,
            coordinate,
            NormalizeDirection(direction) * initialSpeed,
            4,
            damage,
            faction
        ),
        m_Direction(NormalizeDirection(direction)),
        m_InitialSpeed(std::max(0.0F, initialSpeed)),
        m_FinalSpeed(std::max(0.0F, finalSpeed)),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()),
        m_LifetimeMs(std::max(1.0F, lifetimeMs)) {
    }

    void Update() override {
        const Util::ms_t now = Util::Time::GetElapsedTimeMs();
        const float elapsedMs = now - this->m_SpawnTime;
        if (elapsedMs >= this->m_LifetimeMs) {
            this->RequestDestroy();
            return;
        }

        const float t = std::clamp(elapsedMs / this->m_LifetimeMs, 0.0F, 1.0F);
        const float speed =
            this->m_InitialSpeed +
                (this->m_FinalSpeed - this->m_InitialSpeed) * t;
        this->m_Momentum = this->m_Direction * speed;
        Bullet::Update();
    }

private:
    static glm::vec2 NormalizeDirection(const glm::vec2 &direction) {
        if (glm::length(direction) <= 0.0001F) {
            return {1.0F, 0.0F};
        }

        return glm::normalize(direction);
    }

    glm::vec2 m_Direction = {1.0F, 0.0F};
    float m_InitialSpeed = 0.0F;
    float m_FinalSpeed = 0.0F;
    Util::ms_t m_SpawnTime = 0;
    float m_LifetimeMs = 0.0F;
};

class ZulanBombTrapConeBullet : public Bullet {
public:
    ZulanBombTrapConeBullet(
        const glm::vec2 &coordinate,
        const glm::vec2 &direction,
        CombatFaction faction,
        MapSystem* mapSystem
    ) : Bullet(
            kBombTrapConeBulletSprite,
            coordinate,
            NormalizeDirection(direction) * kBombTrapConeBulletSpeed,
            4,
            kBombTrapConeDamage,
            faction
        ),
        m_MapSystem(mapSystem),
        m_Direction(NormalizeDirection(direction)),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()) {
        ConfigureBulletSize(
            *this,
            kZulanConeBulletVisualScale,
            kZulanConeBulletColliderSize
        );
    }

    void Update() override {
        const Util::ms_t now = Util::Time::GetElapsedTimeMs();
        if (now - this->m_SpawnTime >= kBombTrapConeBulletLifetimeMs) {
            this->RequestDestroy();
            return;
        }

        if (this->HasRegisteredImpact() || this->IsDestroyRequested()) {
            return;
        }

        const glm::vec2 previousPosition = this->GetAbsoluteTranslation();
        const float previousTravelDistance = this->m_TravelDistance;

        Bullet::Update();

        if (this->HasRegisteredImpact() || this->IsDestroyRequested()) {
            return;
        }

        const glm::vec2 currentPosition = this->GetAbsoluteTranslation();
        const glm::vec2 segment = currentPosition - previousPosition;
        const float segmentLength = glm::length(segment);
        this->m_TravelDistance += segmentLength;

        while (this->m_NextEmitDistance <= this->m_TravelDistance) {
            const float segmentT =
                segmentLength <= 0.0001F ?
                    1.0F :
                    std::clamp(
                        (this->m_NextEmitDistance - previousTravelDistance) /
                            segmentLength,
                        0.0F,
                        1.0F
                    );
            this->EmitBubblePair(previousPosition + segment * segmentT);
            this->m_NextEmitDistance += kBombTrapConeEmitIntervalDistance;
        }
    }

private:
    static glm::vec2 NormalizeDirection(const glm::vec2 &direction) {
        if (glm::length(direction) <= 0.0001F) {
            return {1.0F, 0.0F};
        }

        return glm::normalize(direction);
    }

    void EmitBubblePair(const glm::vec2 &origin) {
        if (this->m_MapSystem == nullptr) {
            return;
        }

        const glm::vec2 side = {-this->m_Direction.y, this->m_Direction.x};
        for (float sign : {-1.0F, 1.0F}) {
            const glm::vec2 bubbleDirection = side * sign;
            std::shared_ptr<Bullet> bullet =
                std::make_shared<ZulanDeceleratingBullet>(
                    kBombTrapBubbleBulletSprite,
                    origin + bubbleDirection * kBombTrapBubbleSpawnOffset,
                    bubbleDirection,
                    kBombTrapBubbleInitialSpeed,
                    kBombTrapBubbleFinalSpeed,
                    kBombTrapBubbleDamage,
                    this->GetFaction(),
                    kBombTrapBubbleLifetimeMs
                );
            ConfigureBulletSize(
                bullet,
                kZulanBubbleBulletVisualScale,
                kZulanBubbleBulletColliderSize
            );
            this->m_MapSystem->AddBullet(bullet);
        }
    }

    MapSystem* m_MapSystem = nullptr;
    glm::vec2 m_Direction = {1.0F, 0.0F};
    Util::ms_t m_SpawnTime = 0;
    float m_TravelDistance = 0.0F;
    float m_NextEmitDistance = kBombTrapConeEmitInitialDistance;
};

class ZulanFloatingSatelliteOrbBullet : public Bullet {
public:
    ZulanFloatingSatelliteOrbBullet(
        const glm::vec2 &center,
        std::size_t satelliteIndex,
        float startAngle,
        int rotationDirection,
        CombatFaction faction,
        MapSystem* mapSystem,
        Util::ms_t orbitStartTime
    ) : Bullet(
            kPowerfulMotherBulletSprite,
            GetFloatingSatellitePosition(
                center,
                satelliteIndex,
                startAngle,
                rotationDirection,
                0.0F
            ),
            {0.0F, 0.0F},
            kFloatingSatelliteOrbZIndex,
            kFloatingSatelliteOrbDamage,
            faction
        ),
        m_MapSystem(mapSystem),
        m_Center(center),
        m_SatelliteIndex(satelliteIndex),
        m_StartTime(orbitStartTime),
        m_NextEmitTime(
            this->m_StartTime +
                kFloatingSatelliteBarEmitInitialDelayMs +
                static_cast<float>(satelliteIndex) *
                    kFloatingSatelliteBarEmitIntervalMs /
                    static_cast<float>(kCannonCount)
        ),
        m_StartAngle(startAngle),
        m_RotationDirection(rotationDirection) {
        ConfigureBulletSize(
            *this,
            kZulanLightOrbBulletVisualScale,
            kZulanLightOrbBulletColliderSize
        );

        Collision::CollisionFilter filter;
        filter.layer = Collision::CollisionLayer::None;
        filter.mask = Collision::ToMask(Collision::CollisionLayer::None);
        this->SetCollisionFilter(filter);
    }

    void Update() override {
        const Util::ms_t now = Util::Time::GetElapsedTimeMs();
        const float elapsedMs = static_cast<float>(now - this->m_StartTime);

        if (elapsedMs >= kFloatingSatelliteLifetimeMs) {
            this->RequestDestroy();
            return;
        }

        this->UpdateOrbit(elapsedMs);
        this->ApplyContactDamage(now);

        while (now >= this->m_NextEmitTime && !this->IsDestroyRequested()) {
            this->EmitBarBullet(this->m_NextEmitTime);
            this->m_NextEmitTime += kFloatingSatelliteBarEmitIntervalMs;
        }
    }

private:
    void UpdateOrbit(float elapsedMs) {
        const glm::vec2 direction = GetFloatingSatelliteDirection(
            this->m_StartAngle,
            this->m_RotationDirection,
            elapsedMs
        );
        this->SetAbsoluteTranslation(GetFloatingSatellitePosition(
            this->m_Center,
            this->m_SatelliteIndex,
            this->m_StartAngle,
            this->m_RotationDirection,
            elapsedMs
        ));
        this->SetAbsoluteRotation(std::atan2(direction.y, direction.x));
    }

    void ApplyContactDamage(Util::ms_t now) {
        if (this->m_MapSystem == nullptr ||
            now < this->m_NextContactDamageTime) {
            return;
        }

        const glm::vec2 orbPosition = this->GetAbsoluteTranslation();
        const float orbRadius =
            std::max(kZulanLightOrbBulletColliderSize.x, kZulanLightOrbBulletColliderSize.y) *
                0.5F;

        for (const auto &player : this->m_MapSystem->GetPlayers()) {
            if (player == nullptr || player->IsDead()) {
                continue;
            }

            const float playerRadius =
                std::max(player->GetColliderSize().x, player->GetColliderSize().y) *
                    0.5F;
            if (glm::distance(player->GetAbsoluteTranslation(), orbPosition) >
                playerRadius + orbRadius) {
                continue;
            }

            player->ApplyDamage(kFloatingSatelliteOrbDamage);
            this->m_NextContactDamageTime =
                now + kFloatingSatelliteOrbContactDamageIntervalMs;
            return;
        }
    }

    void EmitBarBullet(Util::ms_t emitTime) {
        if (this->m_MapSystem == nullptr) {
            return;
        }

        const float elapsedMs = static_cast<float>(emitTime - this->m_StartTime);
        const glm::vec2 direction = GetFloatingSatelliteDirection(
            this->m_StartAngle,
            this->m_RotationDirection,
            elapsedMs
        );
        const glm::vec2 origin = GetFloatingSatellitePosition(
            this->m_Center,
            this->m_SatelliteIndex,
            this->m_StartAngle,
            this->m_RotationDirection,
            elapsedMs
        );

        std::shared_ptr<Bullet> bullet = std::make_shared<ZulanTimedBullet>(
            kBarBulletSprite,
            origin + direction * kFloatingSatelliteBarSpawnOffset,
            direction * kFloatingSatelliteBarBulletSpeed,
            kFloatingSatelliteBarDamage,
            this->GetFaction(),
            kFloatingSatelliteBarBulletLifetimeMs
        );
        ConfigureBulletSize(
            bullet,
            kZulanBarBulletVisualScale,
            kZulanBarBulletColliderSize
        );
        this->m_MapSystem->AddBullet(bullet);
    }

    MapSystem* m_MapSystem = nullptr;
    glm::vec2 m_Center = {0.0F, 0.0F};
    std::size_t m_SatelliteIndex = 0;
    Util::ms_t m_StartTime = 0;
    Util::ms_t m_NextEmitTime = 0;
    Util::ms_t m_NextContactDamageTime = 0;
    float m_StartAngle = 0.0F;
    int m_RotationDirection = 1;
};

class ZulanVortexMotherBullet : public Bullet {
public:
    ZulanVortexMotherBullet(
        const glm::vec2 &coordinate,
        const glm::vec2 &momentum,
        CombatFaction faction,
        MapSystem* mapSystem,
        float initialVortexAngle
    ) : Bullet(
            kPowerfulMotherBulletSprite,
            coordinate,
            momentum,
            4,
            kPowerfulMotherBulletDamage,
            faction
        ),
        m_MapSystem(mapSystem),
        m_SpawnTime(Util::Time::GetElapsedTimeMs()),
        m_NextEmitTime(
            this->m_SpawnTime + kPowerfulChildBulletEmitInitialDelayMs
        ),
        m_InitialVortexAngle(initialVortexAngle) {
    }

    void Update() override {
        const Util::ms_t now = Util::Time::GetElapsedTimeMs();
        if (now - this->m_SpawnTime >= kPowerfulMotherBulletLifetimeMs) {
            this->RequestDestroy();
            return;
        }

        while (now >= this->m_NextEmitTime && !this->IsDestroyRequested()) {
            this->EmitChildBullets(this->m_NextEmitTime);
            this->m_NextEmitTime += kPowerfulChildBulletEmitIntervalMs;
        }

        Bullet::Update();
    }

private:
    void EmitChildBullets(Util::ms_t emitTime) {
        if (this->m_MapSystem == nullptr) {
            return;
        }

        const float elapsedMs =
            static_cast<float>(emitTime - this->m_SpawnTime);
        const float baseAngle =
            this->m_InitialVortexAngle +
            elapsedMs * kPowerfulChildBulletVortexRadiansPerMs;

        for (int index = 0; index < kPowerfulChildBulletCount; ++index) {
            const float angle =
                baseAngle +
                2.0F * kPi * static_cast<float>(index) /
                    static_cast<float>(kPowerfulChildBulletCount);
            const glm::vec2 direction = {std::cos(angle), std::sin(angle)};

            std::shared_ptr<Bullet> bullet = std::make_shared<ZulanTimedBullet>(
                kPowerfulChildBulletSprite,
                this->GetAbsoluteTranslation() +
                    direction * kPowerfulChildBulletSpawnOffset,
                direction * kPowerfulChildBulletSpeed,
                kPowerfulChildBulletDamage,
                this->GetFaction(),
                kPowerfulChildBulletLifetimeMs
            );
            ConfigureBulletSize(
                bullet,
                kZulanRoundBulletVisualScale,
                kZulanRoundBulletColliderSize
            );
            this->m_MapSystem->AddBullet(bullet);
        }
    }

    MapSystem* m_MapSystem = nullptr;
    Util::ms_t m_SpawnTime = 0;
    Util::ms_t m_NextEmitTime = 0;
    float m_InitialVortexAngle = 0.0F;
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

    this->m_NextSkill = this->RandomSkillKind();
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
            } else if (this->m_NextSkill == SkillKind::InterceptLaser) {
                this->StartInterceptLaser(target);
            } else if (this->m_NextSkill == SkillKind::HeavyArtillery) {
                this->StartHeavyArtillery();
            } else if (this->m_NextSkill == SkillKind::LightArtillery) {
                this->StartLightArtillery();
            } else if (this->m_NextSkill == SkillKind::BombTrap) {
                this->StartBombTrap();
            } else if (this->m_NextSkill == SkillKind::FloatingSatellite) {
                this->StartFloatingSatellite();
            } else if (this->m_NextSkill == SkillKind::PowerfulMotherBullet) {
                this->StartPowerfulMotherBullet();
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

    case BossState::InterceptLaser:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->UpdateInterceptLaser(target);
        break;

    case BossState::HeavyArtillery:
        this->m_MoveIntent = {0.0F, 0.0F};
        break;

    case BossState::LightArtillery:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->UpdateLightArtillery();
        break;

    case BossState::BombTrap:
        this->m_MoveIntent = {0.0F, 0.0F};
        break;

    case BossState::FloatingSatellite:
        this->m_MoveIntent = {0.0F, 0.0F};
        this->UpdateFloatingSatellite();
        break;

    case BossState::PowerfulMotherBullet:
        this->m_MoveIntent = {0.0F, 0.0F};
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
        this->m_NextSkill = this->RandomSkillKind();
        this->m_NextSkillTime = this->m_StateStartTime + kSkillCooldownMs;
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

void ZulanInRuins::StartHeavyArtillery() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetAllCannonBeamsVisible(false);
    this->FireHeavyArtillery();
    this->EnterState(BossState::Cooldown);
}

void ZulanInRuins::FireHeavyArtillery() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 origin = this->GetChestFireOrigin();
    const float startAngle = this->RandomFloat(0.0F, 2.0F * kPi);

    for (int index = 0; index < kHeavyArtilleryBulletCount; ++index) {
        const float angle =
            startAngle +
            2.0F * kPi * static_cast<float>(index) /
                static_cast<float>(kHeavyArtilleryBulletCount);
        const glm::vec2 direction = {std::cos(angle), std::sin(angle)};

        std::shared_ptr<Bullet> bullet = std::make_shared<ZulanTimedBullet>(
            kHeavyArtilleryBulletSprite,
            origin + direction * 20.0F,
            direction * kHeavyArtilleryBulletSpeed,
            kHeavyArtilleryDamage,
            this->GetFaction(),
            kHeavyArtilleryBulletLifetimeMs
        );
        ConfigureBulletSize(
            bullet,
            kZulanRoundBulletVisualScale,
            kZulanRoundBulletColliderSize
        );
        this->m_MapSystem->AddBullet(bullet);
    }
}

void ZulanInRuins::StartLightArtillery() {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_LightArtilleryEndTime = now + kLightArtilleryDurationMs;
    this->m_NextLightArtilleryShotTime = now;
    this->SetAllCannonBeamsVisible(false);
    this->EnterState(BossState::LightArtillery);
}

void ZulanInRuins::UpdateLightArtillery() {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (now >= this->m_NextLightArtilleryShotTime &&
        now < this->m_LightArtilleryEndTime) {
        this->FireLightArtilleryBullet();
        this->m_NextLightArtilleryShotTime =
            now + kLightArtilleryShotIntervalMs;
    }

    if (now >= this->m_LightArtilleryEndTime) {
        this->EnterState(BossState::Cooldown);
    }
}

void ZulanInRuins::FireLightArtilleryBullet() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const float angle = this->RandomFloat(0.0F, 2.0F * kPi);
    const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
    const glm::vec2 origin = this->GetChestFireOrigin();

    std::shared_ptr<Bullet> bullet = std::make_shared<ZulanTimedBullet>(
        kLightArtilleryBulletSprite,
        origin + direction * 16.0F,
        direction * kLightArtilleryBulletSpeed,
        kLightArtilleryDamage,
        this->GetFaction(),
        kLightArtilleryBulletLifetimeMs
    );
    ConfigureBulletSize(
        bullet,
        kZulanMagicBulletVisualScale,
        kZulanMagicBulletColliderSize
    );
    this->m_MapSystem->AddBullet(bullet);
}

void ZulanInRuins::StartBombTrap() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetAllCannonBeamsVisible(false);
    this->EnterState(BossState::BombTrap);
    this->FireBombTrap();
    this->EnterState(BossState::Cooldown);
}

void ZulanInRuins::FireBombTrap() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 baseDirection = this->NormalizeOrFallback(this->m_FaceDirection);
    const float baseAngle = std::atan2(baseDirection.y, baseDirection.x);
    const glm::vec2 origin = this->GetChestFireOrigin();
    const float step =
        kBombTrapConeBulletCount <= 1 ?
            0.0F :
            kBombTrapConeFanRadians /
                static_cast<float>(kBombTrapConeBulletCount - 1);
    const float startAngle = baseAngle - kBombTrapConeFanRadians * 0.5F;

    for (int index = 0; index < kBombTrapConeBulletCount; ++index) {
        const float angle = startAngle + step * static_cast<float>(index);
        const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
        std::shared_ptr<Bullet> bullet =
            std::make_shared<ZulanBombTrapConeBullet>(
                origin + direction * 18.0F,
                direction,
                this->GetFaction(),
                this->m_MapSystem
            );
        this->m_MapSystem->AddBullet(bullet);
    }
}

void ZulanInRuins::StartFloatingSatellite() {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const glm::vec2 facing = this->NormalizeOrFallback(this->m_FaceDirection);

    this->m_MoveIntent = {0.0F, 0.0F};
    this->m_FloatingSatelliteStartTime = now;
    this->m_FloatingSatelliteCenter = this->GetAbsoluteTranslation();
    this->m_FloatingSatelliteStartAngle = std::atan2(facing.y, facing.x);
    this->m_FloatingSatelliteRotationDirection = facing.x >= 0.0F ? 1 : -1;
    this->m_FloatingSatelliteReturning = false;
    this->SetAllCannonBeamsVisible(false);

    for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
        FloatingCannon &cannon = this->m_Cannons[index];
        cannon.state = CannonState::Ready;
        cannon.position = this->GetFloatingSatellitePosition(index, now);
        cannon.fireDirection = this->GetFloatingSatelliteDirection(now);
        cannon.laserDamageApplied = false;
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonActiveZIndex);
        }
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);

        if (this->m_MapSystem != nullptr) {
            std::shared_ptr<Bullet> orb =
                std::make_shared<ZulanFloatingSatelliteOrbBullet>(
                    this->m_FloatingSatelliteCenter,
                    index,
                    this->m_FloatingSatelliteStartAngle,
                    this->m_FloatingSatelliteRotationDirection,
                    this->GetFaction(),
                    this->m_MapSystem,
                    this->m_FloatingSatelliteStartTime
                );
            this->m_MapSystem->AddBullet(orb);
        }
    }

    this->EnterState(BossState::FloatingSatellite);
}

void ZulanInRuins::UpdateFloatingSatellite() {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    const float elapsedMs =
        static_cast<float>(now - this->m_FloatingSatelliteStartTime);

    if (!this->m_FloatingSatelliteReturning &&
        elapsedMs < kFloatingSatelliteLifetimeMs + kFloatingSatelliteCannonDriftMs) {
        for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
            FloatingCannon &cannon = this->m_Cannons[index];
            cannon.state = CannonState::Ready;
            cannon.position = this->GetFloatingSatellitePosition(index, now);
            cannon.fireDirection = this->GetFloatingSatelliteDirection(now);
            if (cannon.visual != nullptr) {
                cannon.visual->SetZIndex(kCannonActiveZIndex);
            }
        }
        return;
    }

    if (!this->m_FloatingSatelliteReturning) {
        this->m_FloatingSatelliteReturning = true;
        for (FloatingCannon &cannon : this->m_Cannons) {
            cannon.state = CannonState::Returning;
            this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
            this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
        }
    }

    for (FloatingCannon &cannon : this->m_Cannons) {
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonStoredZIndex);
        }
        this->MoveCannonToward(
            cannon,
            this->GetAbsoluteTranslation() + cannon.homeOffset
        );
    }

    if (this->AreCannonsAtHome()) {
        this->ResetCannonsToHome();
        this->EnterState(BossState::Cooldown);
    }
}

glm::vec2 ZulanInRuins::GetFloatingSatellitePosition(
    std::size_t index,
    Util::ms_t now
) const {
    const float elapsedMs =
        static_cast<float>(now - this->m_FloatingSatelliteStartTime);
    return ::GetFloatingSatellitePosition(
        this->m_FloatingSatelliteCenter,
        index,
        this->m_FloatingSatelliteStartAngle,
        this->m_FloatingSatelliteRotationDirection,
        elapsedMs
    );
}

glm::vec2 ZulanInRuins::GetFloatingSatelliteDirection(Util::ms_t now) const {
    const float elapsedMs =
        static_cast<float>(now - this->m_FloatingSatelliteStartTime);
    return ::GetFloatingSatelliteDirection(
        this->m_FloatingSatelliteStartAngle,
        this->m_FloatingSatelliteRotationDirection,
        elapsedMs
    );
}

void ZulanInRuins::StartPowerfulMotherBullet() {
    this->m_MoveIntent = {0.0F, 0.0F};
    this->SetAllCannonBeamsVisible(false);
    this->EnterState(BossState::PowerfulMotherBullet);
    this->FirePowerfulMotherBullet();
    this->EnterState(BossState::Cooldown);
}

void ZulanInRuins::FirePowerfulMotherBullet() {
    if (this->m_MapSystem == nullptr) {
        return;
    }

    const glm::vec2 direction = this->NormalizeOrFallback(this->m_FaceDirection);
    const glm::vec2 origin = this->GetChestFireOrigin();

    std::shared_ptr<Bullet> bullet = std::make_shared<ZulanVortexMotherBullet>(
        origin + direction * 18.0F,
        direction * kPowerfulMotherBulletSpeed,
        this->GetFaction(),
        this->m_MapSystem,
        this->RandomFloat(0.0F, 2.0F * kPi)
    );
    ConfigureBulletSize(
        bullet,
        kZulanLightOrbBulletVisualScale,
        kZulanLightOrbBulletColliderSize
    );
    this->m_MapSystem->AddBullet(bullet);
}

void ZulanInRuins::StartInterceptLaser(
    const std::shared_ptr<Character> &target
) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    this->m_InterceptState = InterceptState::MovingToLine;
    this->m_InterceptStepStartTime = now;
    this->m_InterceptRound = 1;
    this->m_InterceptCurrentIndex = 0;
    this->m_InterceptActiveCount = static_cast<int>(this->m_Cannons.size());
    this->m_InterceptSecondRoundPending = this->RandomBool();

    for (FloatingCannon &cannon : this->m_Cannons) {
        cannon.state = CannonState::Approaching;
        cannon.laserDamageApplied = false;
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonActiveZIndex);
        }
        if (target != nullptr) {
            cannon.fireDirection =
                this->GetInterceptFireDirection(cannon, target);
        }
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
    }

    this->EnterState(BossState::InterceptLaser);
}

void ZulanInRuins::UpdateInterceptLaser(
    const std::shared_ptr<Character> &target
) {
    const Util::ms_t now = Util::Time::GetElapsedTimeMs();

    switch (this->m_InterceptState) {
    case InterceptState::MovingToLine: {
        const bool ready = this->MoveCannonsToInterceptLine(target);
        if (ready ||
            now - this->m_StateStartTime >=
                kArrayFormationDelayMs + kInterceptFormationGraceMs) {
            this->StartNextInterceptCannonWarning(target);
        }
        break;
    }

    case InterceptState::Warning: {
        if (this->m_InterceptCurrentIndex < 0 ||
            this->m_InterceptCurrentIndex >= static_cast<int>(this->m_Cannons.size())) {
            this->StartInterceptReturn();
            break;
        }

        FloatingCannon &cannon = this->m_Cannons[this->m_InterceptCurrentIndex];
        if (cannon.warningVisual != nullptr) {
            cannon.warningVisual->ConfigureBeam(
                cannon.position,
                cannon.fireDirection,
                cannon.beamLength,
                kWarningThicknessScale
            );
        }
        if (now - this->m_InterceptStepStartTime >= kCannonLaserWarningMs) {
            this->FireInterceptCannon();
        }
        break;
    }

    case InterceptState::Firing: {
        if (this->m_InterceptCurrentIndex < 0 ||
            this->m_InterceptCurrentIndex >= static_cast<int>(this->m_Cannons.size())) {
            this->StartInterceptReturn();
            break;
        }

        FloatingCannon &cannon = this->m_Cannons[this->m_InterceptCurrentIndex];
        if (cannon.laserVisual != nullptr) {
            cannon.laserVisual->ConfigureBeam(
                cannon.position,
                cannon.fireDirection,
                cannon.beamLength,
                kLaserThicknessScale
            );
        }
        if (now - this->m_InterceptStepStartTime >= kCannonLaserDurationMs) {
            this->FinishInterceptCannon(target);
        }
        break;
    }

    case InterceptState::Returning:
        for (FloatingCannon &cannon : this->m_Cannons) {
            if (cannon.visual != nullptr) {
                cannon.visual->SetZIndex(kCannonStoredZIndex);
            }
            this->MoveCannonToward(
                cannon,
                this->GetAbsoluteTranslation() + cannon.homeOffset
            );
        }
        if (this->AreCannonsAtHome()) {
            this->ResetCannonsToHome();
            this->EnterState(BossState::Cooldown);
        }
        break;
    }
}

bool ZulanInRuins::MoveCannonsToInterceptLine(
    const std::shared_ptr<Character> &target
) {
    bool allReady = true;
    for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
        FloatingCannon &cannon = this->m_Cannons[index];
        const glm::vec2 targetPosition =
            this->GetInterceptLinePosition(index, target);
        this->MoveCannonToward(cannon, targetPosition);
        cannon.fireDirection = this->GetInterceptFireDirection(cannon, target);
        if (glm::distance(cannon.position, targetPosition) > kCannonArrivalDistance) {
            allReady = false;
        }
    }

    return allReady;
}

glm::vec2 ZulanInRuins::GetInterceptLinePosition(
    std::size_t index,
    const std::shared_ptr<Character> &target
) const {
    const glm::vec2 origin = this->GetAbsoluteTranslation();
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
        forward * kInterceptLineForwardOffset +
        side * slot * kInterceptLineSpacing;
}

glm::vec2 ZulanInRuins::GetInterceptFireDirection(
    const FloatingCannon &cannon,
    const std::shared_ptr<Character> &target
) const {
    if (target != nullptr) {
        return this->NormalizeOrFallback(
            target->GetAbsoluteTranslation() - cannon.position
        );
    }

    return this->NormalizeOrFallback(this->m_FaceDirection);
}

void ZulanInRuins::StartNextInterceptCannonWarning(
    const std::shared_ptr<Character> &target
) {
    if (this->m_InterceptCurrentIndex >= this->m_InterceptActiveCount ||
        this->m_InterceptCurrentIndex >= static_cast<int>(this->m_Cannons.size())) {
        if (this->m_InterceptRound == 1 && this->m_InterceptSecondRoundPending) {
            this->StartInterceptSecondRound(target);
            return;
        }

        this->StartInterceptReturn();
        return;
    }

    FloatingCannon &cannon = this->m_Cannons[this->m_InterceptCurrentIndex];
    cannon.fireDirection = this->GetInterceptFireDirection(cannon, target);
    cannon.beamLength = this->ComputeRoomClippedBeamLength(
        cannon.position,
        cannon.fireDirection,
        kLaserFallbackLength
    );
    cannon.laserDamageApplied = false;
    cannon.state = CannonState::WarningLaser;
    cannon.stateStartTime = Util::Time::GetElapsedTimeMs();
    this->m_InterceptStepStartTime = cannon.stateStartTime;
    this->m_InterceptState = InterceptState::Warning;

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

void ZulanInRuins::FireInterceptCannon() {
    if (this->m_InterceptCurrentIndex < 0 ||
        this->m_InterceptCurrentIndex >= static_cast<int>(this->m_Cannons.size())) {
        this->StartInterceptReturn();
        return;
    }

    FloatingCannon &cannon = this->m_Cannons[this->m_InterceptCurrentIndex];
    this->FireCannonLaser(cannon);
    const glm::vec2 laserEnd =
        cannon.position + cannon.fireDirection * cannon.beamLength;
    this->DamagePlayersAlongLaser(cannon.position, laserEnd);
    cannon.laserDamageApplied = true;
    this->m_InterceptStepStartTime = Util::Time::GetElapsedTimeMs();
    this->m_InterceptState = InterceptState::Firing;
}

void ZulanInRuins::FinishInterceptCannon(
    const std::shared_ptr<Character> &target
) {
    if (this->m_InterceptCurrentIndex < 0 ||
        this->m_InterceptCurrentIndex >= static_cast<int>(this->m_Cannons.size())) {
        this->StartInterceptReturn();
        return;
    }

    FloatingCannon &cannon = this->m_Cannons[this->m_InterceptCurrentIndex];
    this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
    this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
    cannon.state = CannonState::Ready;
    ++this->m_InterceptCurrentIndex;
    this->StartNextInterceptCannonWarning(target);
}

void ZulanInRuins::StartInterceptSecondRound(
    const std::shared_ptr<Character> &target
) {
    this->m_InterceptRound = 2;
    this->m_InterceptCurrentIndex = 0;
    const int cannonCount = static_cast<int>(this->m_Cannons.size());
    this->m_InterceptActiveCount =
        this->RandomBool() ?
            std::min(kInterceptSecondRoundFrontCount, cannonCount) :
            cannonCount;

    for (std::size_t index = 0; index < this->m_Cannons.size(); ++index) {
        FloatingCannon &cannon = this->m_Cannons[index];
        cannon.position = this->GetInterceptLinePosition(index, target);
        cannon.fireDirection = this->GetInterceptFireDirection(cannon, target);
        cannon.state = CannonState::Ready;
        cannon.laserDamageApplied = false;
        if (cannon.visual != nullptr) {
            cannon.visual->SetZIndex(kCannonActiveZIndex);
        }
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
    }

    this->StartNextInterceptCannonWarning(target);
}

void ZulanInRuins::StartInterceptReturn() {
    this->m_InterceptState = InterceptState::Returning;
    this->m_InterceptStepStartTime = Util::Time::GetElapsedTimeMs();
    for (FloatingCannon &cannon : this->m_Cannons) {
        cannon.state = CannonState::Returning;
        this->SetBeamVisible(cannon, cannon.warningVisual, cannon.warningAttached, false);
        this->SetBeamVisible(cannon, cannon.laserVisual, cannon.laserAttached, false);
    }
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
        ConfigureBulletSize(
            bullet,
            kZulanBarBulletVisualScale,
            kZulanBarBulletColliderSize
        );
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

glm::vec2 ZulanInRuins::GetChestFireOrigin() const {
    glm::vec2 fireDirection = this->m_FaceDirection;
    if (glm::length(fireDirection) <= 0.0001F) {
        fireDirection = {1.0F, 0.0F};
    }

    return this->GetAbsoluteTranslation() +
        glm::normalize(fireDirection) * kChestFireForwardOffset;
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

ZulanInRuins::SkillKind ZulanInRuins::RandomSkillKind() {
    if (kDebugForceInterceptLaserOnly) {
        return SkillKind::InterceptLaser;
    }

    if (kDebugForceArtilleryOnly) {
        return this->RandomBool() ?
            SkillKind::HeavyArtillery :
            SkillKind::LightArtillery;
    }

    if (kDebugForceBombTrapOnly) {
        return SkillKind::BombTrap;
    }

    if (kDebugForceFloatingSatelliteOnly) {
        return SkillKind::FloatingSatellite;
    }

    if (kDebugForcePowerfulMotherBulletOnly) {
        return SkillKind::PowerfulMotherBullet;
    }

    const int maxSkillIndex = this->m_Phase == Phase::Angry ? 7 : 6;
    std::uniform_int_distribution<int> distribution(0, maxSkillIndex);
    switch (distribution(this->m_RandomEngine)) {
    case 0:
        return SkillKind::AutoChase;
    case 1:
        return SkillKind::ArrayShot;
    case 2:
        return SkillKind::InterceptLaser;
    case 3:
        return SkillKind::HeavyArtillery;
    case 4:
        return SkillKind::LightArtillery;
    case 5:
        return SkillKind::FloatingSatellite;
    case 6:
        return SkillKind::PowerfulMotherBullet;
    default:
        return SkillKind::BombTrap;
    }
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
