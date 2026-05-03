#ifndef ANCIENT_GEAR_SET_HPP
#define ANCIENT_GEAR_SET_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/MapObject.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Util/GameObject.hpp"

class Player;

class AncientGearSetBeamVisual : public Util::GameObject, public MapObject {
public:
    AncientGearSetBeamVisual(const std::string &resource, float zIndex);

    Util::Transform GetObjectTransform() const override;
    void ConfigureBeam(
        const glm::vec2 &start,
        const glm::vec2 &direction,
        float length,
        float thicknessScale
    );
};

class AncientGearSet : public Mob {
public:
    AncientGearSet(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

protected:
    void UpdateWeaponPresentation() override;

private:
    enum class GearState {
        Idle,
        Warning,
        Firing,
        Cooldown
    };

    std::shared_ptr<Character> GetTarget() const;
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;
    void EnterState(GearState state);
    void StartWarning(const std::shared_ptr<Character> &target);
    void FireLaser();
    void DamagePlayersAlongLaser(const glm::vec2 &start, const glm::vec2 &end);
    void KnockbackPlayerFromLaser(Player &player) const;
    void UpdateBeamVisual(
        const std::shared_ptr<AncientGearSetBeamVisual> &visual,
        float thicknessScale
    );
    void SetWarningVisible(bool visible);
    void SetLaserVisible(bool visible);

    GearState m_GearState = GearState::Idle;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_NextAttackTime = 0;
    glm::vec2 m_MoveIntent = {0.0F, 0.0F};
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    glm::vec2 m_LaserDirection = {1.0F, 0.0F};
    float m_LaserLength = 0.0F;
    bool m_LaserDamageApplied = false;
    bool m_WarningAttached = false;
    bool m_LaserAttached = false;
    std::shared_ptr<AncientGearSetBeamVisual> m_WarningVisual;
    std::shared_ptr<AncientGearSetBeamVisual> m_LaserVisual;
};

#endif
