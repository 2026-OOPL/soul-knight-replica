#ifndef CHEST_REWARD_HPP
#define CHEST_REWARD_HPP

#include <functional>
#include <memory>

#include <glm/vec2.hpp>

class MapSystem;
class Player;
class Weapon;

class IChestReward {
public:
    virtual ~IChestReward() = default;

    virtual void Grant(
        Player &player,
        MapSystem &mapSystem,
        const glm::vec2 &dropPosition
    ) = 0;
};

class WeaponChestReward : public IChestReward {
public:
    using WeaponFactory = std::function<std::shared_ptr<Weapon>()>;

    explicit WeaponChestReward(WeaponFactory factory);

    void Grant(
        Player &player,
        MapSystem &mapSystem,
        const glm::vec2 &dropPosition
    ) override;

private:
    WeaponFactory m_Factory;
};

class ConsumableChestReward : public IChestReward {
public:
    enum class Type {
        Heal,
        Ammo,
        Shield
    };

    ConsumableChestReward(Type type, int amount);

    void Grant(
        Player &player,
        MapSystem &mapSystem,
        const glm::vec2 &dropPosition
    ) override;

private:
    Type m_Type = Type::Heal;
    int m_Amount = 0;
};

#endif
