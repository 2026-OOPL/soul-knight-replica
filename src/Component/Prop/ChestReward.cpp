#include <algorithm>
#include <memory>
#include <utility>

#include "Component/Prop/ChestReward.hpp"

#include "Component/Bullet.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Component/Weapon.hpp"

WeaponChestReward::WeaponChestReward(WeaponFactory factory)
    : m_Factory(std::move(factory)) {
}

void WeaponChestReward::Grant(Player &player, MapSystem &mapSystem) {
    if (this->m_Factory == nullptr) {
        return;
    }

    const std::shared_ptr<Weapon> weapon = this->m_Factory();
    if (weapon == nullptr) {
        return;
    }

    weapon->SetOnBulletFired(
        [&mapSystem](std::shared_ptr<Bullet> bullet) {
            mapSystem.AddBullet(std::move(bullet));
        }
    );
    player.SetWeapon(weapon);
}

ConsumableChestReward::ConsumableChestReward(Type type, int amount)
    : m_Type(type),
      m_Amount(std::max(0, amount)) {
}

void ConsumableChestReward::Grant(Player &player, MapSystem &mapSystem) {
    (void)mapSystem;

    switch (this->m_Type) {
    case Type::Heal:
        player.Heal(this->m_Amount);
        return;

    case Type::Ammo:
        player.RecoverAmmo(this->m_Amount);
        return;

    case Type::Shield:
        player.RestoreShield(this->m_Amount);
        return;
    }
}
