#ifndef VITAMIN_C_MECHA_HPP
#define VITAMIN_C_MECHA_HPP

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

class VitaminCBombEffect;

class VitaminCMechaBeamVisual : public Util::GameObject, public MapObject {
public:
    VitaminCMechaBeamVisual(const std::string &resource, float zIndex);

    Util::Transform GetObjectTransform() const override;
    void ConfigureBeam(
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float length,
        float thicknessScale
    );
};

class VitaminCMecha : public Mob {
public:
    VitaminCMecha(
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
        Idle,
        SpiralCannon,
        LaserCannon,
        MachineGun,
        DisintegrationCannon,
        Bombardment,
        Cooldown
    };

    enum class SkillKind {
        SpiralCannon,
        LaserCannon,
        MachineGun,
        DisintegrationCannon,
        Bombardment
    };

    void UpdatePhase();
    void EnterAngryPhase();
    void UpdateState();
    void EnterState(BossState state);
    void UpdateSlowMovement(const std::shared_ptr<Character> &target);
    void PruneBombEffects();

    void StartSpiralCannon();
    void UpdateSpiralCannon();
    void FireOneSpiralBullet();

    void StartLaserCannon();
    void UpdateLaserCannon();
    void ConfigureLaserBeam(
        const std::shared_ptr<VitaminCMechaBeamVisual> &beam,
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float thicknessScale
    );
    void SetLaserBeamVisible(
        const std::shared_ptr<VitaminCMechaBeamVisual> &beam,
        bool &attached,
        bool visible
    );
    void HideLaserBeams();
    void DamagePlayersAlongLaser(const glm::vec2 &start, const glm::vec2 &end);

    void StartMachineGun();
    void UpdateMachineGun(const std::shared_ptr<Character> &target);
    void FireMachineGunBullet(const std::shared_ptr<Character> &target);

    void StartDisintegrationCannon();
    void UpdateDisintegrationCannon(const std::shared_ptr<Character> &target);
    void FireDisintegrationBullet(const std::shared_ptr<Character> &target);

    void StartBombardment();
    void UpdateBombardment(const std::shared_ptr<Character> &target);
    void SpawnBombWarning(const std::shared_ptr<Character> &target);

    glm::vec2 GetCenterFireOrigin() const;
    glm::vec2 GetHandFireOrigin(int handIndex) const;
    float ComputeRoomClippedBeamLength(
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float fallbackLength
    ) const;
    std::shared_ptr<Character> GetTarget() const;
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;
    SkillKind RandomSkillKind();
    int RandomInt(int minValue, int maxValue);
    float RandomFloat(float minValue, float maxValue);

    Phase m_Phase = Phase::Normal;
    BossState m_BossState = BossState::Idle;
    SkillKind m_NextSkill = SkillKind::MachineGun;
    std::shared_ptr<Util::Animation> m_NormalAnimation;
    std::shared_ptr<Util::Animation> m_AngryAnimation;
    std::shared_ptr<VitaminCMechaBeamVisual> m_LeftLaserVisual;
    std::shared_ptr<VitaminCMechaBeamVisual> m_RightLaserVisual;
    std::vector<std::shared_ptr<VitaminCBombEffect>> m_BombEffects;
    bool m_LeftLaserAttached = false;
    bool m_RightLaserAttached = false;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_NextSkillTime = 0;
    Util::ms_t m_NextStrafeFlipTime = 0;
    Util::ms_t m_NextSpiralShotTime = 0;
    Util::ms_t m_NextMachineGunShotTime = 0;
    Util::ms_t m_DisintegrationFireTime = 0;
    Util::ms_t m_NextBombTime = 0;
    Util::ms_t m_NextLaserDamageTime = 0;
    glm::vec2 m_MoveIntent = {0.0F, 0.0F};
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    std::mt19937 m_RandomEngine;
    float m_SpiralAngle = 0.0F;
    int m_StrafeDirection = 1;
    int m_SpiralShotsFired = 0;
    int m_SpiralTotalShots = 0;
    int m_MachineGunShotsFired = 0;
    int m_MachineGunTotalShots = 0;
    int m_BombsSpawned = 0;
    int m_TotalBombs = 0;
    bool m_DisintegrationFired = false;
};

#endif
