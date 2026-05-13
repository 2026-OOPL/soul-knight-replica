#include "Component/Bullets/ConfiguredBullet.hpp"

#include <memory>

#include "Util/Animation.hpp"

namespace {

std::shared_ptr<Util::Animation> BuildAnimation(const BulletConfig &config) {
    if (config.sprites.empty()) {
        return nullptr;
    }

    return std::make_shared<Util::Animation>(
        config.sprites,
        config.loopAnimation,
        config.frameIntervalMs
    );
}

} // namespace

ConfiguredBullet::ConfiguredBullet(
    const BulletConfig &config,
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction
) : Bullet(
        BuildAnimation(config),
        coordinate,
        momentum,
        config.zIndex,
        damage,
        faction
    ) {
    this->SetVisualScalePreservingFlip(config.visualScale);
    this->SetColliderSize(config.colliderSize);
}
