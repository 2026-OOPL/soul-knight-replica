#ifndef SHEAR_HPP
#define SHEAR_HPP

#include <memory>
#include <string>
#include <vector>

#include "Common/MapObject.hpp"
#include "Component/Bullet.hpp"
#include "Component/IStateful.hpp"
#include "Component/Weapon.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"

namespace {
    const std::string SHEAR_WEAPON_SKIN = RESOURCE_DIR"/Weapon/Shear.png";
    const std::vector<std::string> SHEAR_SPIKE_EFFECT = {
        RESOURCE_DIR"/Effect/Spike/Spike_0.png",
        RESOURCE_DIR"/Effect/Spike/Spike_1.png",
        RESOURCE_DIR"/Effect/Spike/Spike_2.png"
    };

    constexpr int kShearFireDelayMs = 900;
    constexpr int kShearDamage = 3;
    constexpr float kShearThrustDistance = 24.0F;
    constexpr float kShearThrustDurationMs = 160.0F;
    constexpr float kShearEffectDurationMs = 120.0F;
}

class ShearAttackEffect : public Util::GameObject, public MapObject, public IStateful {
public:
    ShearAttackEffect();

    void Update() override;
    Util::Transform GetObjectTransform() const override;

    void Play(const glm::vec2 &anchor, const glm::vec2 &facingDirection);
    void SyncToWeapon(
        const glm::vec2 &anchor,
        const glm::vec2 &facingDirection,
        float distanceFromAnchor
    );

private:
    std::shared_ptr<Util::Animation> m_Animation;
    glm::vec2 m_FacingDirection = {1.0F, 0.0F};
    float m_EndTime = 0.0F;
    bool m_Active = false;
};

class Shear : public Weapon {
public:
    Shear();

    void Update() override;
    bool ShotBullet() override;
    WeaponType GetWeaponType() override { return WeaponType::MELEE; }

protected:
    std::shared_ptr<Bullet> CreateBullet() const override;

private:
    void TriggerThrust();
    float GetThrustOffset() const;
    void SyncSpikeEffect();

    std::shared_ptr<ShearAttackEffect> m_SpikeEffect;
    float m_ThrustStartTime = 0.0F;
    float m_ThrustEndTime = 0.0F;
};

#endif
