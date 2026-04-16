#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <functional>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <string>
#include <vector>

#include <glm/fwd.hpp>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"

#include "Common/CombatFaction.hpp"
#include "Common/MapObject.hpp"
#include "Component/Collision/ICollidable.hpp"
#include "Component/IStateful.hpp"
#include "Component/Weapon.hpp"

class Bullet;

class Character : public MapObject,
                  public IStateful,
                  public Util::GameObject,
                  public ICollidable {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const ICollidable &,
        const glm::vec2 &
    )>;

    Character(
        std::shared_ptr<Util::Animation> StandAnimation,
        std::shared_ptr<Util::Animation> WalkAnimation,
        std::shared_ptr<Util::Animation> DieAnimation,
        int zIndex,
        CombatFaction faction
    );

    Character(
        const std::vector<std::string>& StandSprite,
        const std::vector<std::string>& WalkSprite,
        const std::vector<std::string>& DieSprite,
        int zIndex,
        CombatFaction faction
    );

    // Override for MapObject
    Util::Transform GetObjectTransform() const override; 
    const std::shared_ptr<Core::Drawable> &GetDebugDrawable() const {
        return this->m_Drawable;
    }
    bool IsDebugVisible() const {
        return this->m_Visible;
    }

    std::shared_ptr<Weapon> GetWeapon();
    virtual void SetWeapon(std::shared_ptr<Weapon> weapon);

    int GetCurrentHealth() const;
    int GetMaxHealth() const;
    void SetCurrentHealth(int health);
    void SetMaxHealth(int maxHealth);
    CombatFaction GetFaction() const;
    void SetFaction(CombatFaction faction);
    virtual void ApplyDamage(int damage);
    void Heal(int amount);
    bool IsDead() const;

    void Update() override;

    glm::vec2 GetCollisionOrigin() const override;
    const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const override;
    void OnCollision(const Collision::CollisionSituation &situation) override;

    glm::vec2 GetColliderSize() const;
    void SetColliderSize(const glm::vec2 &colliderSize);
    Collision::CollisionFilter GetCollisionFilter() const;
    void SetCollisionFilter(const Collision::CollisionFilter &filter);
    Collision::AxisAlignedBox GetCollisionBox() const;
    Collision::AxisAlignedBox GetCollisionBoxAt(const glm::vec2 &coordinate) const;
    void SetCollisionResolver(CollisionResolver collisionResolver);

    virtual glm::vec2 GetMoveIntent() const = 0;
    virtual glm::vec2 GetFaceDirection() const = 0;

    glm::vec2 m_LastMomentum;
    
protected:
    virtual bool CanBeDamagedBy(const Bullet &bullet) const;
    void SetAttackAnimation(std::shared_ptr<Util::Animation> attackAnimation);
    void TriggerAttackVisual(float durationMs = 120.0F);
    bool IsAttackVisualActive() const;

    int m_CurrentHealth = 10;
    int m_MaxHealth = 10;
    CombatFaction m_Faction = CombatFaction::Neutral;
    
    float m_PlayerSpeed = 0.15F;

    std::shared_ptr<Weapon> m_Weapon;
 
    std::shared_ptr<Util::Animation> m_DieAnimation;
    std::shared_ptr<Util::Animation> m_StandAnimation;
    std::shared_ptr<Util::Animation> m_WalkAnimation;
    std::shared_ptr<Util::Animation> m_AttackAnimation;
    std::vector<Collision::CollisionBox> m_CollisionBoxes;
    CollisionResolver m_CollisionResolver = nullptr;
    Util::ms_t m_AttackVisualEndTime = 0;

private:

    void UpdateFaceDirection();
    void SetSpriteTypeByMoveIntent(glm::vec2 moveIntent);
};

#endif
