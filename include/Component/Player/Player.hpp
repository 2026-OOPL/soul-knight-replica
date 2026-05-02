#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include <array>
#include <functional>
#include <nlohmann/detail/iterators/primitive_iterator.hpp>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"

struct PlayerHudState {
    int hp;
    int maxHp;
    int shield;
    int maxShield;
    int ammo;
    int maxAmmo;
};

class Player : public Character {
public:
    using MeleeAttackResolver = std::function<bool(Player &)>;

    Player(
        const std::vector<std::string>& StandSprite,
        const std::vector<std::string>& WalkSprite,
        const std::vector<std::string>& DieSprite,
        int maxHealth = 10,
        int maxShield = 5,
        int maxAmmo = 100
    );

    virtual glm::vec2 GetMoveIntent() const override;
    void SetWeapon(std::shared_ptr<Weapon> weapon) override;
    void SetOnWeaponBulletFired(std::function<void(std::shared_ptr<Bullet>)> callback);
    void SetMeleeAttackResolver(MeleeAttackResolver resolver);
    void ApplyDamage(int damage) override;
    
    int GetCurrentShield() const;
    int GetMaxShield() const;
    void SetCurrentShield(int shield);
    void SetMaxShield(int maxShield);
    void RestoreShield(int amount);
    
    int GetCurrentAmmo() const;
    int GetMaxAmmo() const;
    void SetCurrentAmmo(int ammo);
    void SetMaxAmmo(int maxAmmo);
    bool TryConsumeAmmo(int amount);
    void RecoverAmmo(int amount);
    
    PlayerHudState GetHudState() const;
    
    void Update() override;
    
protected:
    float GetMoveSpeedMultiplier() const override;

private:
    void SwitchWeapon();
    void EquipWeaponSlot(int slotIndex);
    void ApplyWeaponBulletCallback(const std::shared_ptr<Weapon> &weapon);
    bool TryMeleeAttack();
    bool IsMeleeAttacking() const;
    void StartMeleeAttackVisual();
    void UpdateMeleeAttackVisual();

    float m_ShieldRegenDelayRemainingMs = 0.0F;
    float m_ShieldRegenElapsedMs = 0.0F;
    int m_CurrentShield = 0;
    int m_MaxShield = 0;
    int m_CurrentAmmo = 0;
    int m_MaxAmmo = 0;
    std::array<std::shared_ptr<Weapon>, 2> m_WeaponSlots = {nullptr, nullptr};
    int m_ActiveWeaponSlot = 0;
    std::function<void(std::shared_ptr<Bullet>)> m_OnWeaponBulletFired;
    MeleeAttackResolver m_MeleeAttackResolver = nullptr;
    std::shared_ptr<Util::Animation> m_MeleeAttackAnimation;
    std::shared_ptr<Util::GameObject> m_MeleeAttackVisual;
    float m_LastMeleeAttackTime = -1000.0F;
    float m_MeleeAttackVisualEndTime = 0.0F;

    static constexpr float kShieldRegenDelayMs = 1000.0F;
    static constexpr float kShieldRegenIntervalMs = 1000.0F;
    static constexpr float kMeleeAttackCooldownMs = 350.0F;
    static constexpr float kMeleeAttackVisualDurationMs = 140.0F;
};

#endif
