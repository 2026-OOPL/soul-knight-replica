#ifndef GHOST_HPP
#define GHOST_HPP

#include <memory>
#include <random>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Util/Time.hpp"

class Ghost : public Mob {
public:
    Ghost(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

protected:
    void UpdateWeaponPresentation() override;

private:
    enum class GhostState {
        Active,
        AttackRecover,
        Hidden
    };

    std::shared_ptr<Character> GetTarget() const;
    void UpdateMovement(const std::shared_ptr<Character> &target);
    void FireRadialBarrage();
    void EnterHidden();
    void SetHidden(bool hidden);
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;
    int RandomInt(int minValue, int maxValue);
    float RandomFloat(float minValue, float maxValue);

    GhostState m_GhostState = GhostState::Active;
    Util::ms_t m_StateStartTime = 0;
    Util::ms_t m_HiddenEndTime = 0;
    Util::ms_t m_NextAttackTime = 0;
    Util::ms_t m_NextStrafeFlipTime = 0;
    glm::vec2 m_MoveIntent = {0.0F, 0.0F};
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    std::mt19937 m_RandomEngine;
    int m_StrafeDirection = 1;
    bool m_Hidden = false;
};

#endif
