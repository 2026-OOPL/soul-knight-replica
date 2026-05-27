#ifndef DROPPED_WEAPON_HPP
#define DROPPED_WEAPON_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Prop/Prop.hpp"

class MapSystem;
class Player;
class Weapon;

class DroppedWeapon : public Prop {
public:
    struct Config {
        float pickupRadius = 30.0F;
        int zIndex = 3;
    };

    DroppedWeapon(const glm::vec2 &coordinate, std::shared_ptr<Weapon> weapon);
    DroppedWeapon(
        const glm::vec2 &coordinate,
        std::shared_ptr<Weapon> weapon,
        Config config
    );

    void Initialize(MapSystem *mapSystem) override;
    void Update() override;

private:
    std::shared_ptr<Player> FindPlayerInPickupRange() const;
    std::shared_ptr<Weapon> TakeWeapon();

    MapSystem *m_MapSystem = nullptr;
    std::shared_ptr<Weapon> m_Weapon = nullptr;
    float m_PickupRadius = 30.0F;
};

#endif
