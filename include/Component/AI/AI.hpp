#ifndef AI_HPP
#define AI_HPP

#include <glm/vec2.hpp>

#include "Common/Random.hpp"
#include "Component/AI/StateMachine.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IStateful.hpp"
#include "Util/Time.hpp"

enum class Status {
    WANDER,
    FRIGHTENED,
    PURSUIT,
    STOPANDATTACK,
    MAX = STOPANDATTACK
};

namespace AIConfig {
    /// If the moving delta is less than this value, then we assume that ai is stucked, 
    /// so we consider this is an arrived destination behavior 
    constexpr float ARRIVE_MOVING_DISTANCE = 0.005F;

    /// The radius we consider that ai is arrived to the desired translation
    constexpr float ARRIVE_DESTINATION_DISTANCE = 1.0F;

    /// The radius of the mob standing in attack status
    constexpr float ATTACK_MINIMAL_RADIUS = 50.0F;
    constexpr float ATTACK_MAXIMAL_RADIUS = 90.0F;

    /// The maximum random angle that apply to the mob while attacking 
    /// Which simulate the handshake
    // TODO...

    /// 
    constexpr float PURSUIT_RADUIS_ANGLE = M_PI / 3.0F;

    constexpr float STATE_PERSUIT_MIN_RANGE = 130.0F;

    constexpr float STATE_STOPANDATTACK_MIN_RANGE = 50.0F;
    constexpr float STATE_STOPANDATTACK_MAX_RANGE = 120.0F;

    constexpr float FRIGHTENED_WAIT_TIME = 3000;
}

class AI : public IStateful {
public:
    AI(
        Character* owner,
        std::shared_ptr<Character> target,
        Collision::CollisionSystem* collision
    );

    virtual ~AI() override = default;
    
    bool GetAttackTrigger();
    bool IsArriveDestinaton();

    virtual glm::vec2 GetMoveDirection() = 0;
    virtual glm::vec2 GetFaceDirection() = 0;
    virtual glm::vec2 GetAttackDirection() = 0;

    void Freeze();
    void UnFreeze();

protected:
    /// The mob which AI control to
    Character* m_Owner;

    /// The target to attack 
    std::shared_ptr<Character> m_Target;

    /// Needed for path finding
    Collision::CollisionSystem* m_CollisionSystem;

    /// The place that AI is chasing to get to
    glm::vec2 m_DesiredTranslation;

    /// Current state machine of the AI
    StateMachine m_StateMachine = StateMachine(Status::WANDER);

    /// The last facing of the mob
    glm::vec2  m_LastFacing;
    glm::vec2  m_LastPosition;
    Util::ms_t m_LastUpdateTime = 0;

    RandomChoose m_Random;

    bool m_Freezed = false;

    glm::vec2 GetFaceToTarget();
    float     GetDistanceToTarget();

    /// Return the vactor after applied the angle between [-angle/2, angle/2]
    glm::vec2 ApplyRandomAngle(glm::vec2 vector, float angle);
    /// Correct move direction by the information of the 3-way tenetacles check
    glm::vec2 ApplyObstacleAvoidance(const glm::vec2& currentPos, const glm::vec2& desiredDir);

    Status GetNextState();

private:
    /// Check if the desired place is occupied by obastacles
    bool IsPointBlocked(const glm::vec2& point);

    void UpdateDesiredTranslation();
};

#endif // AI_HPP
