#ifndef AI_HPP
#define AI_HPP

#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <random>

#include "Common/Random.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IStateful.hpp"
#include "Component/Player/Player.hpp"
#include "Util/Time.hpp"

enum class Status {
    WANDER, 
    PURSUIT,
    STOPANDATTACK
};

namespace AIConfig {
    /// If the moving delta is less than this value, then we assume that ai is stucked, 
    /// so we consider this is an arrived destination behavior 
    constexpr float ARRIVE_MOVING_DISTANCE = 0.005F;

    /// The radius we consider that ai is arrived to the desired translation
    constexpr float ARRIVE_DESTINATION_DISTANCE = 1.0F;
}

class AI : public IStateful {
public:
    AI(
        Character* owner,
        std::shared_ptr<Character> target,
        Collision::CollisionSystem* collision
    ) {
      this->m_Owner = owner;
      this->m_Target = target;
      this->m_CollisionSystem = collision;

      m_Random = RandomChoose();
    };

    ~AI() override = default;
    
    void Update() override;
    
    bool GetAttackTrigger();
    bool IsArriveDestinaton();

    virtual glm::vec2 GetMoveDirection();
    virtual glm::vec2 GetFaceDirection();
    virtual glm::vec2 GetAttackDirection();

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
    Status m_Status = Status::WANDER;

    /// The last facing of the mob
    glm::vec2  m_LastFacing;
    glm::vec2  m_LastPosition;
    Util::ms_t m_LastUpdateTime = 0;

    RandomChoose m_Random;

    bool m_Freezed = false;

    glm::vec2 GetFacingToTarget();
    float     GetDistanceToTarget();

private:
    /// Correct move direction by the information of the 3-way tenetacles check
    glm::vec2 ApplyObstacleAvoidance(const glm::vec2& currentPos, const glm::vec2& desiredDir);
    
    /// Check if the desired place is occupied by obastacles
    bool IsPointBlocked(const glm::vec2& point); 


    void UpdateDesiredDirection();

    glm::vec2 CalculateStopAndAttack(glm::vec2 ownerPos, glm::vec2 targetPos);

    
};

#endif //
