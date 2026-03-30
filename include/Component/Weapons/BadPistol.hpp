#include <string>

#include "Component/Weapon.hpp"

namespace {
    const std::string WEAPON_SKIN = RESOURCE_DIR"/Weapon/BadPistol.png";
};

class BadPistol : public Weapon {
public:
    BadPistol() : Weapon(
        WEAPON_SKIN,
        300
    ) {};

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }
};