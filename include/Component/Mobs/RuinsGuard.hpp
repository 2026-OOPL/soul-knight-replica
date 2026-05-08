#ifndef RUINS_GUARD_HPP
#define RUINS_GUARD_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionTypes.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Common/MapObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Time.hpp"

namespace Core {
class Drawable;
}

class Player;

class RuinsGuardEquipmentVisual : public Util::GameObject, public MapObject {
public:
    RuinsGuardEquipmentVisual(
        const std::shared_ptr<Core::Drawable> &drawable,
        float zIndex
    ) : Util::GameObject(drawable, zIndex) {
    }

    Util::Transform GetObjectTransform() const override {
        return this->m_Transform;
    }
};

namespace {
    const std::vector<std::string> RUINS_GUARD_WALK_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_1.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_2.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_walk_3.png"
    };

    const std::vector<std::string> RUINS_GUARD_STAND_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_0.png",
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_stand_1.png"
    };

    const std::vector<std::string> RUINS_GUARD_DIE_SPRITE = {
        RESOURCE_DIR"/Mob/RuinsGuard/RuinsGuard_die.png"
    };
}

class RuinsGuard : public Mob {
public:
    RuinsGuard(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;
    void ApplyDamage(int damage) override;

protected:
    void UpdateWeaponPresentation() override;

private:
    enum class GuardState {
        Idle,
        Pursuit,
        PikeAttack,
        Defense,
        ChargeWindup,
        Charging,
        ChargeRecover,
        RadialShot
    };

    enum class CycleAttack {
        Charge,
        RadialShot
    };

    void UpdateState();
    void EnterState(GuardState state);
    void UpdateEquipmentPresentation();
    void UpdateIdleEquipmentPresentation();
    void UpdateCombatEquipmentPresentation();
    void UpdateDefenseEquipmentPresentation();
    void UpdateChargeEquipmentPresentation();
    void SetChargeEffectActive(bool active);
    void TryStartAttackByDistance(float distanceToTarget);
    void StartPikeAttack(const glm::vec2 &targetPosition);
    void StartDefense();
    void StartChargeWindup(const glm::vec2 &targetPosition);
    void StartRadialShot();
    void UpdatePikeAttack();
    void UpdateDefense();
    void UpdateChargeWindup();
    void UpdateCharging();
    void UpdateChargeRecover();
    void FirePikeHitbox();
    void FireRadialBullets();
    void DamagePlayerOnChargeContact();
    void KnockbackPlayerFromGuard(Player &player, float strength) const;
    std::shared_ptr<Character> GetTarget() const;
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;

    std::shared_ptr<RuinsGuardEquipmentVisual> m_PikeVisual;
    std::shared_ptr<RuinsGuardEquipmentVisual> m_ShieldVisual;
    std::shared_ptr<RuinsGuardEquipmentVisual> m_ChargeEffectVisual;
    std::shared_ptr<Util::Animation> m_ChargeEffectAnimation;

    GuardState m_GuardState = GuardState::Idle;
    CycleAttack m_NextCycleAttack = CycleAttack::Charge;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_NextDecisionTime = 0;
    Util::ms_t m_LastContactDamageTime = 0;
    glm::vec2 m_MoveIntent = {0.0F, 0.0F};
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    glm::vec2 m_ChargeVelocity = {0.0F, 0.0F};
    glm::vec2 m_PikeAttackDirection = {1.0F, 0.0F};
    bool m_PikeHitboxFired = false;
    bool m_RadialShotFired = false;
    bool m_ChargeEffectAttached = false;
    int m_ChargeBounceCount = 0;
};

#endif
