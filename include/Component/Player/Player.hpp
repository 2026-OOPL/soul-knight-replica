#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

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
    
private:
    float m_ShieldRegenDelayRemainingMs = 0.0F;
    float m_ShieldRegenElapsedMs = 0.0F;
    int m_CurrentShield = 0;
    int m_MaxShield = 0;
    int m_CurrentAmmo = 0;
    int m_MaxAmmo = 0;

    static constexpr float kShieldRegenDelayMs = 1000.0F;
    static constexpr float kShieldRegenIntervalMs = 1000.0F;
};

#endif
