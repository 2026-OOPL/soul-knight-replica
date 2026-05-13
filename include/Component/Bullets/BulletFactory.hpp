#ifndef BULLET_FACTORY_HPP
#define BULLET_FACTORY_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Common/CombatFaction.hpp"
#include "Component/Bullets/BulletConfig.hpp"

class Bullet;

class BulletFactory {
public:
    static const BulletConfig &SmallNormalBulletConfig();
    static const BulletConfig &ArrowBulletConfig();
    static const BulletConfig &PoopBulletConfig();
    static const BulletConfig &EnemyRoundBulletConfig();
    static const BulletConfig &EnemyLaserBulletConfig();

    static void ApplyConfig(Bullet &bullet, const BulletConfig &config);

    static std::shared_ptr<Bullet> CreateTestBullet(
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage = 1,
        CombatFaction faction = CombatFaction::Neutral
    );

    static std::shared_ptr<Bullet> CreateSmallNormalBullet(
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage = 1,
        CombatFaction faction = CombatFaction::Neutral
    );

    static std::shared_ptr<Bullet> CreateArrowBullet(
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage = 1,
        CombatFaction faction = CombatFaction::Neutral
    );

    static std::shared_ptr<Bullet> CreateSpinningPoopBullet(
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage = 1,
        CombatFaction faction = CombatFaction::Neutral
    );

    static std::shared_ptr<Bullet> CreateEnemyRoundTimedBullet(
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int damage,
        CombatFaction faction,
        float lifetimeMs
    );
};

#endif
