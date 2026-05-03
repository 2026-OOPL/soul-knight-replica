#ifndef BAD_PISTOL_HPP
#define BAD_PISTOL_HPP

#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "Component/Bullet.hpp"
#include "Component/Weapon.hpp"

namespace {
    const std::string WEAPON_SKIN = RESOURCE_DIR"/Weapon/BadPistol.png";
    const std::vector<std::string> BAD_PISTOL_BULLET_SPRITE = {
        RESOURCE_DIR"/Bullet/SmallNormalBullet.png"
    };
    constexpr int kBadPistolFireDelayMs = 333;
    constexpr int kBadPistolDamage = 3;
    constexpr int kBadPistolAmmoCost = 0;
    constexpr int kBadPistolCriticalChance = 0;
    constexpr float kBadPistolAttackOffsetDegrees = 5.0F;
    constexpr float kBadPistolAttackMoveSpeedMultiplier = 0.9F;

    glm::vec2 ApplyBadPistolAttackOffset(const glm::vec2 &direction) {
        static std::random_device randomDevice;
        static std::mt19937 engine(randomDevice());
        static std::uniform_real_distribution<float> distribution(
            -kBadPistolAttackOffsetDegrees,
            kBadPistolAttackOffsetDegrees
        );

        constexpr float kDegreesToRadians = 3.14159265358979323846F / 180.0F;
        const float angle =
            std::atan2(direction.y, direction.x) + distribution(engine) * kDegreesToRadians;
        return {std::cos(angle), std::sin(angle)};
    }
}

class BadPistol : public Weapon {
public:
    BadPistol() : Weapon(
        WEAPON_SKIN,
        kBadPistolFireDelayMs
    ) {
        this->SetBulletDamage(kBadPistolDamage);
        this->SetAmmoCostPerShot(kBadPistolAmmoCost);
        this->SetCriticalChance(kBadPistolCriticalChance);
        this->SetAttackMoveSpeedMultiplier(kBadPistolAttackMoveSpeedMultiplier);
        this->SetMuzzleOffset({18.0F, 0.0F});
    }

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }

protected:
    std::shared_ptr<Bullet> CreateBullet() const override {
        std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(
            BAD_PISTOL_BULLET_SPRITE,
            this->GetMuzzlePoint(),
            ApplyBadPistolAttackOffset(this->m_FacingDirection),
            1,
            this->GetBulletDamage(),
            this->GetProjectileFaction()
        );
        return bullet;
    }
};

#endif
