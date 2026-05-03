#ifndef PLUNGER_HPP
#define PLUNGER_HPP

#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

#include "Component/Bullets/SpinningBullet.hpp"
#include "Component/Weapon.hpp"

namespace {
    const std::string PLUNGER_WEAPON_SKIN = RESOURCE_DIR"/Weapon/Plunger.png";
    const std::vector<std::string> PLUNGER_BULLET_SPRITE = {
        RESOURCE_DIR"/Bullet/PoopBullet.png"
    };

    constexpr int kPlungerFireDelayMs = 333;
    constexpr int kPlungerDamage = 5;
    constexpr int kPlungerAmmoCost = 2;
    constexpr int kPlungerCriticalChance = 20;
    constexpr float kPlungerAttackOffsetDegrees = 10.0F;
    constexpr float kPlungerBulletSpeed = 0.9F;
    constexpr float kPlungerAttackMoveSpeedMultiplier = 1.1F;

    glm::vec2 ApplyPlungerAttackOffset(const glm::vec2 &direction) {
        static std::random_device randomDevice;
        static std::mt19937 engine(randomDevice());
        static std::uniform_real_distribution<float> distribution(
            -kPlungerAttackOffsetDegrees,
            kPlungerAttackOffsetDegrees
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

    bool RollPlungerCritical(int criticalChance) {
        static std::random_device randomDevice;
        static std::mt19937 engine(randomDevice());
        static std::uniform_int_distribution<int> distribution(1, 100);
        return distribution(engine) <= criticalChance;
    }
}

class Plunger : public Weapon {
public:
    Plunger() : Weapon(
        PLUNGER_WEAPON_SKIN,
        kPlungerFireDelayMs
    ) {
        this->SetBulletDamage(kPlungerDamage);
        this->SetAmmoCostPerShot(kPlungerAmmoCost);
        this->SetCriticalChance(kPlungerCriticalChance);
        this->SetAttackMoveSpeedMultiplier(kPlungerAttackMoveSpeedMultiplier);
        this->SetHoldOffset({4.0F, 0.0F});
        this->SetMuzzleOffset({24.0F, 0.0F});
    }

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }

protected:
    std::shared_ptr<Bullet> CreateBullet() const override {
        const glm::vec2 bulletDirection =
            ApplyPlungerAttackOffset(this->m_FacingDirection);
        const bool isCritical = RollPlungerCritical(this->GetCriticalChance());
        const int damage = this->GetBulletDamage() * (isCritical ? 2 : 1);

        std::shared_ptr<Bullet> bullet = std::make_shared<SpinningBullet>(
            PLUNGER_BULLET_SPRITE,
            this->GetMuzzlePoint(),
            bulletDirection * kPlungerBulletSpeed,
            1,
            damage,
            this->GetProjectileFaction()
        );
        bullet->SetColliderSize({16.0F, 16.0F});
        return bullet;
    }
};

#endif
