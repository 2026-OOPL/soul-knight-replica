#ifndef MELEE_AI_HPP
#define MELEE_AI_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/AI/AI.hpp"

struct MeleeAIConfig {
    float aggroRange = 520.0F;
    float attackRange = 72.0F;
    float attackReleaseRange = 88.0F;
    float pursuitUpdateIntervalMs = 180.0F;
    float arrivalDistance = 4.0F;
};

class MeleeAI : public AI {
public:
    MeleeAI(
        Character* owner,
        std::shared_ptr<Character> target,
        Collision::CollisionSystem* collision,
        MeleeAIConfig config = {}
    );

    glm::vec2 GetMoveDirection() override;
    glm::vec2 GetFaceDirection() override;
    glm::vec2 GetAttackDirection() override;

    void Update() override;

protected:
    Status GetNextState();
    bool NeedUpdate();

    glm::vec2 CalculateDesiredTranslation();

private:
    glm::vec2 NormalizeOrFallback(const glm::vec2 &direction) const;

    MeleeAIConfig m_Config;
};

#endif
