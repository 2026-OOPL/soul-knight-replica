#ifndef SPINNING_BULLET_HPP
#define SPINNING_BULLET_HPP

#include <cmath>
#include <string>
#include <vector>

#include "Component/Bullet.hpp"
#include "Util/Time.hpp"

class SpinningBullet : public Bullet {
public:
    SpinningBullet(
        const std::vector<std::string> &sprite,
        glm::vec2 coordinate,
        glm::vec2 momentum,
        int zIndex,
        int damage = 1,
        CombatFaction faction = CombatFaction::Neutral,
        float spinRadiansPerMs = kDefaultSpinRadiansPerMs
    ) : Bullet(sprite, coordinate, momentum, zIndex, damage, faction),
        m_SpinRadiansPerMs(spinRadiansPerMs) {
    }

    void Update() override {
        Bullet::Update();
        this->SetAbsoluteRotation(
            this->GetAbsoluteRotation() +
            this->m_SpinRadiansPerMs * Util::Time::GetDeltaTimeMs()
        );
    }

private:
    static constexpr float kPi = 3.14159265358979323846F;
    static constexpr float kDefaultSpinRadiansPerMs = (2.0F * kPi) / 500.0F;

    float m_SpinRadiansPerMs;
};

#endif
