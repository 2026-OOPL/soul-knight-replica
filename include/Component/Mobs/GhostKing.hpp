#ifndef GHOST_KING_HPP
#define GHOST_KING_HPP

#include <memory>
#include <random>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Util/Time.hpp"

class GhostKingWarningSign;

class GhostKing : public Mob {
public:
    GhostKing(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    void ApplyDamage(int damage) override;
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
        IntroHidden,
        Idle,
        Cooldown,
        RelocationWarning,
        RelocationHidden,
        EyeVolley,
        ScarletFlame,
        SpiritBurst
    };

    enum class SkillKind {
        EyeVolley,
        ScarletFlame,
        SpiritBurst
    };

    void StartIntroHidden();
    void FinishIntroHidden();
    void UpdatePhase();
    void EnterAngryPhase();
    void UpdateState();
    void EnterState(BossState state);
    void StartCooldown();
    void StartCooldownOrRelocate();
    void StartRelocation();
    void UpdateRelocationWarning();
    void UpdateRelocationHidden();
    void SetIncorporeal(bool incorporeal);

    void UpdateSlowMovement(const std::shared_ptr<Character> &target);
    void StartEyeVolley(const std::shared_ptr<Character> &target);
    void UpdateEyeVolley();
    void FireEyeVolleyGroup();
    void StartScarletFlame(const std::shared_ptr<Character> &target);
    void UpdateScarletFlame();
    void FireScarletBullet();
    void StartSpiritBurst();
    void UpdateSpiritBurst();
    void FireSpiritBurst();

    std::shared_ptr<Character> GetTarget() const;
    glm::vec2 GetFaceFireOrigin() const;
    glm::vec2 GetBurstOrigin() const;
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;
    glm::vec2 RotateVector(const glm::vec2 &vector, float radians) const;
    glm::vec2 FindSafeBossPosition();
    glm::vec2 FindSafeGhostPosition(const glm::vec2 &fallback);
    void ShowRelocationWarning(bool visible);
    void SpawnGhosts(int count);
    void SpawnGhostAt(const glm::vec2 &position);
    SkillKind RandomSkillKind();
    int RandomInt(int minValue, int maxValue);
    float RandomFloat(float minValue, float maxValue);

    Phase m_Phase = Phase::Normal;
    BossState m_BossState = BossState::IntroHidden;
    SkillKind m_NextSkill = SkillKind::EyeVolley;
    std::shared_ptr<Util::Animation> m_NormalAnimation;
    std::shared_ptr<Util::Animation> m_AttackAnimation;
    std::shared_ptr<Util::Animation> m_PressureAnimation;
    std::shared_ptr<GhostKingWarningSign> m_RelocationWarningVisual;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_StateEndTime = 0;
    Util::ms_t m_NextSkillTime = 0;
    Util::ms_t m_NextStrafeFlipTime = 0;
    Util::ms_t m_NextSkillActionTime = 0;
    glm::vec2 m_MoveIntent = {0.0F, 0.0F};
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    glm::vec2 m_LockedSkillDirection = {1.0F, 0.0F};
    glm::vec2 m_RelocationTarget = {0.0F, 0.0F};
    std::mt19937 m_RandomEngine;
    int m_StrafeDirection = 1;
    int m_EyeVolleyGroupsFired = 0;
    int m_ScarletShotsFired = 0;
    int m_ScarletTotalShots = 0;
    int m_BurstRoundsFired = 0;
    int m_BurstTotalRounds = 0;
    bool m_BattleStarted = false;
    bool m_Incorporeal = false;
    bool m_WarningAttached = false;
};

#endif
