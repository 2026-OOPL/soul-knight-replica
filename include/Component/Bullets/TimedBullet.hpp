#ifndef TIMED_BULLET_HPP
#define TIMED_BULLET_HPP

#include <glm/vec2.hpp>

#include "Common/CombatFaction.hpp"
#include "Component/Bullets/BulletConfig.hpp"
#include "Component/Bullets/ConfiguredBullet.hpp"

class TimedBullet : public ConfiguredBullet {
public:
    TimedBullet(
        const BulletConfig &config,
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    );

    void Update() override;

private:
    float m_SpawnTime = 0.0F;
    float m_LifetimeMs = 0.0F;
};

#endif
