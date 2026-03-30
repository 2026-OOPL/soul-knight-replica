#include "Component/Weapon/Weapon.hpp"
#include <string>
#include <vector>

namespace {
    const std::string WEAPON_SKIN = RESOURCE_DIR"/Weapon/BadPistol.png";
};

class BadPistol : public Weapon {
public:
    BadPistol() : Weapon(WEAPON_SKIN) {};

    WeaponType GetWeaponType() override { return WeaponType::RANGED; }
};