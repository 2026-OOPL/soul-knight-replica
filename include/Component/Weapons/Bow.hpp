#ifndef BOW_HPP
#define BOW_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Bullet.hpp"
#include "Component/Weapon.hpp"

namespace {
    const std::string BOW_WEAPON_SKIN = RESOURCE_DIR"/Weapon/Bow.png";
    const std::vector<std::string> BOW_ARROW_SPRITE = {
        RESOURCE_DIR"/Bullet/Arrow.png"
    };

    constexpr int kBowFireDelayMs = 1200;
    constexpr int kBowDamage = 4;
    constexpr int kBowAmmoCost = 0;
    constexpr float kBowArrowSpeed = 1.1F;
}

class Bow : public Weapon {
public:
    Bow() : Weapon(
        BOW_WEAPON_SKIN,
        kBowFireDelayMs
    ) {
        this->SetBulletDamage(kBowDamage);
        this->SetAmmoCostPerShot(kBowAmmoCost);
        this->SetMuzzleOffset({24.0F, 0.0F});
    }

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }

protected:
    std::shared_ptr<Bullet> CreateBullet() const override {
        std::shared_ptr<Bullet> arrow = std::make_shared<Bullet>(
            BOW_ARROW_SPRITE,
            this->GetMuzzlePoint(),
            this->m_FacingDirection * kBowArrowSpeed,
            1,
            this->GetBulletDamage(),
            this->GetProjectileFaction()
        );
        arrow->SetColliderSize({18.0F, 8.0F});
        this->ConfigureBullet(arrow);
        return arrow;
    }
};

#endif
