#ifndef CONFIGURED_BULLET_HPP
#define CONFIGURED_BULLET_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Common/CombatFaction.hpp"
#include "Component/Bullet.hpp"
#include "Component/Bullets/BulletConfig.hpp"

class ConfiguredBullet : public Bullet {
public:
    ConfiguredBullet(
        const BulletConfig &config,
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage = 1,
        CombatFaction faction = CombatFaction::Neutral
    );
};

#endif
