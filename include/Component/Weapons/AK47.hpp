#ifndef AK47_HPP
#define AK47_HPP

#include <cmath>
#include <memory>
#include <random>
#include <string>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

#include "Component/Bullets/BulletFactory.hpp"
#include "Component/Weapon.hpp"

namespace {
    const std::string AK47_WEAPON_SKIN = RESOURCE_DIR"/Weapon/ak47.png";

    constexpr int kAK47FireDelayMs = 167;
    constexpr int kAK47Damage = 3;
    constexpr int kAK47AmmoCost = 1;
    constexpr int kAK47CriticalChance = 12;
    constexpr float kAK47AttackOffsetDegrees = 10.0F;
    constexpr float kAK47AttackMoveSpeedMultiplier = 0.9F;

    glm::vec2 ApplyAK47AttackOffset(const glm::vec2 &direction) {
        static std::random_device randomDevice;
        static std::mt19937 engine(randomDevice());
        static std::uniform_real_distribution<float> distribution(
            -kAK47AttackOffsetDegrees,
            kAK47AttackOffsetDegrees
        );

        constexpr float kDegreesToRadians = 3.14159265358979323846F / 180.0F;
        const glm::vec2 forward =
            glm::length(direction) <= 0.0001F ?
            glm::vec2(1.0F, 0.0F) :
            glm::normalize(direction);
        const float angle =
            std::atan2(forward.y, forward.x) + distribution(engine) * kDegreesToRadians;
        return {std::cos(angle), std::sin(angle)};
    }

    bool RollAK47Critical(int criticalChance) {
        static std::random_device randomDevice;
        static std::mt19937 engine(randomDevice());
        static std::uniform_int_distribution<int> distribution(1, 100);
        return distribution(engine) <= criticalChance;
    }
}

class AK47 : public Weapon {
public:
    AK47() : Weapon(
        AK47_WEAPON_SKIN,
        kAK47FireDelayMs,
        WeaponId::AK47
    ) {
        this->SetBulletDamage(kAK47Damage);
        this->SetAmmoCostPerShot(kAK47AmmoCost);
        this->SetCriticalChance(kAK47CriticalChance);
        this->SetAttackMoveSpeedMultiplier(kAK47AttackMoveSpeedMultiplier);
        this->SetMuzzleOffset({24.0F, 0.0F});
        this->SetShootSFX(RESOURCE_DIR"/SFX/ak47shoot.mp3");
    }

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }

protected:
    std::shared_ptr<Bullet> CreateBullet() const override {
        const glm::vec2 bulletDirection =
            ApplyAK47AttackOffset(this->m_FacingDirection);
        const bool isCritical = RollAK47Critical(this->GetCriticalChance());
        const int damage = this->GetBulletDamage() * (isCritical ? 2 : 1);

        return BulletFactory::CreateSmallNormalBullet(
            this->GetMuzzlePoint(),
            bulletDirection,
            damage,
            this->GetProjectileFaction()
        );
    }
};

#endif
