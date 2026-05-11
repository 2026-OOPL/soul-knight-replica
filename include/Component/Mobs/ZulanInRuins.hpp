#ifndef ZULAN_IN_RUINS_HPP
#define ZULAN_IN_RUINS_HPP

#include <memory>
#include <random>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/MapObject.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

namespace Core {
class Drawable;
}

class Player;

class ZulanInRuinsCannonVisual : public Util::GameObject, public MapObject {
public:
    ZulanInRuinsCannonVisual(
        const std::shared_ptr<Core::Drawable> &drawable,
        float zIndex
    );

    Util::Transform GetObjectTransform() const override;
};

class ZulanInRuinsBeamVisual : public Util::GameObject, public MapObject {
public:
    ZulanInRuinsBeamVisual(const std::string &resource, float zIndex);

    Util::Transform GetObjectTransform() const override;
    void ConfigureBeam(
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float length,
        float thicknessScale
    );
};

class ZulanInRuins : public Mob {
public:
    ZulanInRuins(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    void ApplyDamage(int damage) override;
    void SetAbsoluteTranslation(glm::vec2 transform) override;
    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

protected:
    void UpdateWeaponPresentation() override;

private:
    enum class Phase {
        Normal,
        Angry
    };

    enum class BossState {
        Idle,
        AutoChase,
        ArrayShot,
        InterceptLaser,
        HeavyArtillery,
        LightArtillery,
        BombTrap,
        FloatingSatellite,
        PowerfulMotherBullet,
        Cooldown
    };

    enum class SkillKind {
        AutoChase,
        ArrayShot,
        InterceptLaser,
        HeavyArtillery,
        LightArtillery,
        BombTrap,
        FloatingSatellite,
        PowerfulMotherBullet
    };

    enum class ArrayFormation {
        Breakthrough,
        Rotating
    };

    enum class ArrayAttackMode {
        BarBurst,
        Laser
    };

    enum class CannonState {
        Home,
        Approaching,
        Ready,
        WarningLaser,
        FiringLaser,
        Returning
    };

    enum class InterceptState {
        MovingToLine,
        Warning,
        Firing,
        Returning
    };

    struct FloatingCannon {
        std::shared_ptr<ZulanInRuinsCannonVisual> visual;
        std::shared_ptr<ZulanInRuinsBeamVisual> warningVisual;
        std::shared_ptr<ZulanInRuinsBeamVisual> laserVisual;
        glm::vec2 homeOffset = {0.0F, 0.0F};
        glm::vec2 chaseOffset = {0.0F, 0.0F};
        glm::vec2 position = {0.0F, 0.0F};
        glm::vec2 fireDirection = {1.0F, 0.0F};
        float beamLength = 0.0F;
        Util::ms_t stateStartTime = 0;
        Util::ms_t nextAttackTime = 0;
        int attacksRemaining = 0;
        bool warningAttached = false;
        bool laserAttached = false;
        bool laserDamageApplied = false;
        CannonState state = CannonState::Home;
    };

    void UpdatePhase();
    void EnterAngryPhase();
    void UpdateState();
    void EnterState(BossState state);
    void UpdateSlowMovement(const std::shared_ptr<Character> &target);
    void StartAutoChase(const std::shared_ptr<Character> &target);
    void UpdateAutoChase(const std::shared_ptr<Character> &target);
    void StartArrayShot(const std::shared_ptr<Character> &target);
    void UpdateArrayShot(const std::shared_ptr<Character> &target);
    void StartInterceptLaser(const std::shared_ptr<Character> &target);
    void UpdateInterceptLaser(const std::shared_ptr<Character> &target);
    void StartHeavyArtillery();
    void FireHeavyArtillery();
    void StartLightArtillery();
    void UpdateLightArtillery();
    void FireLightArtilleryBullet();
    void StartBombTrap();
    void FireBombTrap();
    void StartFloatingSatellite();
    void UpdateFloatingSatellite();
    glm::vec2 GetFloatingSatellitePosition(std::size_t index, Util::ms_t now) const;
    glm::vec2 GetFloatingSatelliteDirection(Util::ms_t now) const;
    void StartPowerfulMotherBullet();
    void FirePowerfulMotherBullet();
    bool MoveCannonsToInterceptLine(const std::shared_ptr<Character> &target);
    glm::vec2 GetInterceptLinePosition(std::size_t index, const std::shared_ptr<Character> &target) const;
    glm::vec2 GetInterceptFireDirection(const FloatingCannon &cannon, const std::shared_ptr<Character> &target) const;
    void StartNextInterceptCannonWarning(const std::shared_ptr<Character> &target);
    void FireInterceptCannon();
    void FinishInterceptCannon(const std::shared_ptr<Character> &target);
    void StartInterceptSecondRound(const std::shared_ptr<Character> &target);
    void StartInterceptReturn();
    void UpdateBreakthroughArrayShot(const std::shared_ptr<Character> &target);
    void UpdateRotatingArrayShot();
    void StartArrayReturn();
    bool AreCannonsAtHome() const;
    bool AreCannonsAtArrayFormation(const std::shared_ptr<Character> &target);
    glm::vec2 GetArrayFormationPosition(std::size_t index, const std::shared_ptr<Character> &target) const;
    glm::vec2 GetArrayFireDirection(std::size_t index, const std::shared_ptr<Character> &target) const;
    void TryStartArrayRound(const std::shared_ptr<Character> &target);
    void StartArrayLaserWarning(const std::shared_ptr<Character> &target);
    void FireArrayLaser();
    void FinishArrayRound();
    bool IsArrayRoundBusy() const;
    bool AreCannonsHome() const;
    void UpdateCannon(FloatingCannon &cannon, const std::shared_ptr<Character> &target);
    void UpdateCannonVisual(FloatingCannon &cannon);
    void MoveCannonToward(FloatingCannon &cannon, const glm::vec2 &targetPosition);
    void StartCannonAttack(FloatingCannon &cannon, const std::shared_ptr<Character> &target);
    void FireCannonBarBurst(FloatingCannon &cannon, const std::shared_ptr<Character> &target);
    void FireCannonBarBurstInDirection(FloatingCannon &cannon, const glm::vec2 &direction);
    void StartCannonLaserWarning(FloatingCannon &cannon, const std::shared_ptr<Character> &target);
    void StartCannonLaserWarningInDirection(FloatingCannon &cannon, const glm::vec2 &direction);
    void FireCannonLaser(FloatingCannon &cannon);
    void FinishCannonAttack(FloatingCannon &cannon);
    void SetBeamVisible(
        FloatingCannon &cannon,
        const std::shared_ptr<ZulanInRuinsBeamVisual> &visual,
        bool &attached,
        bool visible
    );
    void SetAllCannonBeamsVisible(bool visible);
    void ResetCannonsToHome();
    void CleanupCannonVisuals();
    void DamagePlayersAlongLaser(const glm::vec2 &start, const glm::vec2 &end);
    glm::vec2 GetChestFireOrigin() const;
    float ComputeRoomClippedBeamLength(
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float fallbackLength
    ) const;
    std::shared_ptr<Character> GetTarget() const;
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;
    glm::vec2 RandomChaseOffset();
    SkillKind RandomSkillKind();
    int RandomAttackCount();
    float RandomFloat(float minValue, float maxValue);
    bool RandomBool();

    Phase m_Phase = Phase::Normal;
    BossState m_BossState = BossState::Idle;
    SkillKind m_NextSkill = SkillKind::ArrayShot;
    ArrayFormation m_ArrayFormation = ArrayFormation::Breakthrough;
    ArrayAttackMode m_ArrayAttackMode = ArrayAttackMode::BarBurst;
    InterceptState m_InterceptState = InterceptState::MovingToLine;
    std::vector<FloatingCannon> m_Cannons;
    std::shared_ptr<Util::Animation> m_NormalAnimation;
    std::shared_ptr<Util::Animation> m_AngryAnimation;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_NextSkillTime = 0;
    Util::ms_t m_NextStrafeFlipTime = 0;
    Util::ms_t m_NextArrayShotTime = 0;
    Util::ms_t m_ArrayRoundStartTime = 0;
    Util::ms_t m_InterceptStepStartTime = 0;
    Util::ms_t m_LightArtilleryEndTime = 0;
    Util::ms_t m_NextLightArtilleryShotTime = 0;
    Util::ms_t m_FloatingSatelliteStartTime = 0;
    glm::vec2 m_MoveIntent = {0.0F, 0.0F};
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    glm::vec2 m_FloatingSatelliteCenter = {0.0F, 0.0F};
    std::mt19937 m_RandomEngine;
    float m_ArrayRotationAngle = 0.0F;
    float m_FloatingSatelliteStartAngle = 0.0F;
    float m_ArrayHorizontalDirection = 1.0F;
    int m_StrafeDirection = 1;
    int m_ArrayRotationDirection = 1;
    int m_FloatingSatelliteRotationDirection = 1;
    int m_ArrayShotsFired = 0;
    int m_ArrayTotalShots = 1;
    int m_InterceptCurrentIndex = 0;
    int m_InterceptActiveCount = 0;
    int m_InterceptRound = 1;
    bool m_ArrayReturning = false;
    bool m_ArrayLaserWarningActive = false;
    bool m_ArrayLaserActive = false;
    bool m_InterceptSecondRoundPending = false;
    bool m_FloatingSatelliteReturning = false;
    bool m_CannonVisualsCleaned = false;
};

#endif
