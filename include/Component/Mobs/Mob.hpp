#ifndef MOB_HPP
#define MOB_HPP

#include <memory>
#include <vector>

#include "Common/CombatFaction.hpp"
#include "Component/AI.hpp"
#include "Component/Character/Character.hpp"

class MapSystem;

class Mob : public Character {
public:
    Mob(
        const std::vector<std::string>& StandAnimation,
        const std::vector<std::string>& WalkAnimation,
        const std::vector<std::string>& DieAnimation,
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Character(
        StandAnimation,
        WalkAnimation,
        DieAnimation,
        4,
        CombatFaction::Enemy
    ) {
        m_TracePlayerTemp = tracePlayer;
        m_CollisionSystemTemp = collisionSystem;

        this->m_AI = std::make_shared<AI>(this, m_TracePlayerTemp, m_CollisionSystemTemp);

        this->m_PlayerSpeed = 0.05F;
    };

    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

    void Update() override;
    void ApplyDamage(int damage) override;
    void SetDamageEnabled(bool enabled);
    bool IsDamageEnabled() const;
    void SetMeleeAttack(
        int damage,
        float range,
        float cooldownMs,
        float knockbackStrength = 0.0F
    );
    bool GetMeleeAttackDebugBox(Collision::AxisAlignedBox &box) const;
    
    std::shared_ptr<AI> m_AI = nullptr;

    void Initialize(MapSystem* mapSystem);
    
protected:
    void UpdateWeaponPresentation() override;
    std::weak_ptr<Character> m_TracePlayerTemp;
    Collision::CollisionSystem* m_CollisionSystemTemp;
    bool TryMeleeAttack();

    glm::vec2 m_FacingDirection;

    MapSystem* m_MapSystem;
    bool m_DamageEnabled = true;
    int m_MeleeDamage = 0;
    float m_MeleeRange = 0.0F;
    float m_MeleeCooldownMs = 800.0F;
    float m_MeleeKnockbackStrength = 0.0F;
    Util::ms_t m_LastMeleeAttackTime = 0;
    Collision::AxisAlignedBox m_MeleeAttackDebugBox;
    Util::ms_t m_MeleeAttackDebugEndTime = 0;
};

#endif
