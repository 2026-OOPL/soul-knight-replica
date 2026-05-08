#ifndef BULLET_HIT_EFFECT_HPP
#define BULLET_HIT_EFFECT_HPP

class Bullet;
class Character;

class IBulletHitEffect {
public:
    virtual ~IBulletHitEffect() = default;

    virtual void Apply(Character &target, const Bullet &bullet) const = 0;
};

class KnockbackHitEffect : public IBulletHitEffect {
public:
    explicit KnockbackHitEffect(float strength);

    void Apply(Character &target, const Bullet &bullet) const override;

private:
    float m_Strength = 0.0F;
};

#endif
