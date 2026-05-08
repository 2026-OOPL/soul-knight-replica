#ifndef RUINS_TURRET_HPP
#define RUINS_TURRET_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/MapObject.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

class RuinsTurretBeamVisual : public Util::GameObject, public MapObject {
public:
    RuinsTurretBeamVisual(const std::string &resource, float zIndex);

    Util::Transform GetObjectTransform() const override;
    void ConfigureBeam(
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float length,
        float thicknessScale
    );
};

class RuinsTurret : public Mob {
public:
    RuinsTurret(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

protected:
    void UpdateWeaponPresentation() override;

private:
    enum class TurretState {
        Idle,
        Warning,
        Firing,
        Cooldown
    };

    std::shared_ptr<Character> GetTarget() const;
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;
    void EnterState(TurretState state);
    void StartWarning(const std::shared_ptr<Character> &target);
    void FireLasers();
    void UpdateWarningVisuals();
    float ComputeRoomClippedWarningLength(const glm::vec2 &direction) const;
    void SetWarningVisible(bool visible);

    TurretState m_TurretState = TurretState::Idle;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_NextAttackTime = 0;
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    glm::vec2 m_FireDirection = {1.0F, 0.0F};
    float m_PositiveWarningLength = 0.0F;
    float m_NegativeWarningLength = 0.0F;
    bool m_LasersFired = false;
    bool m_PositiveWarningAttached = false;
    bool m_NegativeWarningAttached = false;
    std::shared_ptr<RuinsTurretBeamVisual> m_PositiveWarningVisual;
    std::shared_ptr<RuinsTurretBeamVisual> m_NegativeWarningVisual;
};

#endif
