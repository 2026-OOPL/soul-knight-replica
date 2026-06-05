#ifndef SNIPER_RIFLE_HPP
#define SNIPER_RIFLE_HPP

#include <memory>
#include <random>
#include <string>

#include "Component/Bullets/BulletFactory.hpp"
#include "Component/Weapon.hpp"

namespace {
    const std::string SNIPER_RIFLE_WEAPON_SKIN = RESOURCE_DIR"/Weapon/sniper_rifle.png";

    constexpr int kSniperRifleFireDelayMs = 833;
    constexpr int kSniperRifleDamage = 10;
    constexpr int kSniperRifleAmmoCost = 3;
    constexpr int kSniperRifleCriticalChance = 45;
    constexpr float kSniperRifleBulletSpeed = 1.5F;
    constexpr float kSniperRifleAttackMoveSpeedMultiplier = 0.9F;

    bool RollSniperRifleCritical(int criticalChance) {
        static std::random_device randomDevice;
        static std::mt19937 engine(randomDevice());
        static std::uniform_int_distribution<int> distribution(1, 100);
        return distribution(engine) <= criticalChance;
    }
}

class SniperRifle : public Weapon {
public:
    SniperRifle() : Weapon(
        SNIPER_RIFLE_WEAPON_SKIN,
        kSniperRifleFireDelayMs
    ) {
        this->SetBulletDamage(kSniperRifleDamage);
        this->SetAmmoCostPerShot(kSniperRifleAmmoCost);
        this->SetCriticalChance(kSniperRifleCriticalChance);
        this->SetAttackMoveSpeedMultiplier(kSniperRifleAttackMoveSpeedMultiplier);
        this->SetHoldOffset({4.0F, 0.0F});
        this->SetMuzzleOffset({30.0F, 0.0F});
    }

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }

protected:
    std::shared_ptr<Bullet> CreateBullet() const override {
        const bool isCritical = RollSniperRifleCritical(this->GetCriticalChance());
        const int damage = this->GetBulletDamage() * (isCritical ? 2 : 1);

        return BulletFactory::CreateSpindleBullet(
            this->GetMuzzlePoint(),
            this->m_FacingDirection * kSniperRifleBulletSpeed,
            damage,
            this->GetProjectileFaction()
        );
    }
};

#endif
