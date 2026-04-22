#ifndef BAD_PISTOL_HPP
#define BAD_PISTOL_HPP

#include <memory>
#include <string>

#include "Component/Bullet.hpp"
#include "Component/BulletHitEffect.hpp"
#include "Component/Weapon.hpp"

namespace {
    const std::string WEAPON_SKIN = RESOURCE_DIR"/Weapon/BadPistol.png";
    constexpr float kBadPistolKnockbackStrength = 0.1F;
}

class BadPistol : public Weapon {
public:
    BadPistol() : Weapon(
        WEAPON_SKIN,
        300
    ) {
    }

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }

protected:
    void ConfigureBullet(const std::shared_ptr<Bullet> &bullet) const override {
        if (bullet == nullptr) {
            return;
        }

        bullet->AddHitEffect(
            std::make_shared<KnockbackHitEffect>(kBadPistolKnockbackStrength)
        );
    }
};

#endif
