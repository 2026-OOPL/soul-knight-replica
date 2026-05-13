#include "Component/Bullets/TimedBullet.hpp"

#include "Util/Time.hpp"

TimedBullet::TimedBullet(
    const BulletConfig &config,
    glm::vec2 coordinate,
    glm::vec2 momentum,
    int damage,
    CombatFaction faction,
    float lifetimeMs
) : ConfiguredBullet(config, coordinate, momentum, damage, faction),
    m_SpawnTime(Util::Time::GetElapsedTimeMs()),
    m_LifetimeMs(lifetimeMs) {
}

void TimedBullet::Update() {
    if (Util::Time::GetElapsedTimeMs() - this->m_SpawnTime >= this->m_LifetimeMs) {
        this->RequestDestroy();
        return;
    }

    ConfiguredBullet::Update();
}
