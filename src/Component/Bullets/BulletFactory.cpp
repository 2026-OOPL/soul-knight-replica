#include "Component/Bullets/BulletFactory.hpp"

#include <cmath>
#include <memory>

#include "Component/Bullet.hpp"
#include "Component/Bullets/ConfiguredBullet.hpp"
#include "Component/Bullets/SpinningBullet.hpp"
#include "Component/Bullets/TestBullet.hpp"
#include "Component/Bullets/TimedBullet.hpp"

namespace {

constexpr float kDefaultSpinRadiansPerMs =
    (2.0F * 3.14159265358979323846F) / 500.0F;

} // namespace

const BulletConfig &BulletFactory::SmallNormalBulletConfig() {
    static const BulletConfig config = {
        {RESOURCE_DIR"/Bullet/SmallNormalBullet.png"},
        {0.5F, 0.5F},
        {12.0F, 12.0F},
        1,
        true,
        20
    };
    return config;
}

const BulletConfig &BulletFactory::ArrowBulletConfig() {
    static const BulletConfig config = {
        {RESOURCE_DIR"/Bullet/Arrow.png"},
        {1.0F, 1.0F},
        {18.0F, 8.0F},
        1,
        true,
        20
    };
    return config;
}

const BulletConfig &BulletFactory::PoopBulletConfig() {
    static const BulletConfig config = {
        {RESOURCE_DIR"/Bullet/PoopBullet.png"},
        {1.0F, 1.0F},
        {16.0F, 16.0F},
        1,
        true,
        20
    };
    return config;
}

const BulletConfig &BulletFactory::EnemyRoundBulletConfig() {
    static const BulletConfig config = {
        {RESOURCE_DIR"/Bullet/EnemyRoundBullet.png"},
        {1.0F, 1.0F},
        {12.0F, 12.0F},
        1,
        true,
        20
    };
    return config;
}

const BulletConfig &BulletFactory::EnemyLaserBulletConfig() {
    static const BulletConfig config = {
        {RESOURCE_DIR"/Bullet/EnemyLaserBullet.png"},
        {0.5F, 0.5F},
        {15.0F, 4.0F},
        4,
        true,
        20
    };
    return config;
}

void BulletFactory::ApplyConfig(Bullet &bullet, const BulletConfig &config) {
    bullet.SetVisualScalePreservingFlip(config.visualScale);
    bullet.SetColliderSize(config.colliderSize);
}

std::shared_ptr<Bullet> BulletFactory::CreateTestBullet(
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction
) {
    return std::make_shared<TestBullet>(
        coordinate,
        momentum,
        damage,
        faction
    );
}

std::shared_ptr<Bullet> BulletFactory::CreateSmallNormalBullet(
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction
) {
    return std::make_shared<ConfiguredBullet>(
        SmallNormalBulletConfig(),
        coordinate,
        momentum,
        damage,
        faction
    );
}

std::shared_ptr<Bullet> BulletFactory::CreateArrowBullet(
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction
) {
    return std::make_shared<ConfiguredBullet>(
        ArrowBulletConfig(),
        coordinate,
        momentum,
        damage,
        faction
    );
}

std::shared_ptr<Bullet> BulletFactory::CreateSpinningPoopBullet(
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction
) {
    const BulletConfig &config = PoopBulletConfig();
    std::shared_ptr<Bullet> bullet = std::make_shared<SpinningBullet>(
        config.sprites,
        coordinate,
        momentum,
        config.zIndex,
        damage,
        faction,
        kDefaultSpinRadiansPerMs
    );
    ApplyConfig(*bullet, config);
    return bullet;
}

std::shared_ptr<Bullet> BulletFactory::CreateEnemyRoundTimedBullet(
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction,
    float lifetimeMs
) {
    return std::make_shared<TimedBullet>(
        EnemyRoundBulletConfig(),
        coordinate,
        momentum,
        damage,
        faction,
        lifetimeMs
    );
}
