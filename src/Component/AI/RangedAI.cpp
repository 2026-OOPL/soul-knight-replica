#include "Component/AI/RangedAI.hpp"
#include <glm/ext/vector_float2.hpp>
#include <stdexcept>

RangedAI::RangedAI(
    Character* owner,
    std::shared_ptr<Character> target,
    Collision::CollisionSystem* collision
) : AI(owner, target, collision) {

}

glm::vec2 RangedAI::GetMoveDirection() {
    if (m_Freezed) {
        return glm::vec2(0.0f);
    }

    Status currentState = m_StateMachine.GetState();

    if (currentState == Status::FRIGHTENED) {
        return glm::vec2(0.0f);
    }

    if (currentState == Status::STOPANDATTACK) {
        return glm::vec2(0.0f);
    }
    glm::vec2 ownerPos = m_Owner->GetAbsoluteTranslation();

    glm::vec2 finalDir = ApplyObstacleAvoidance(
        ownerPos,
        m_DesiredTranslation - ownerPos);

        finalDir = glm::normalize(finalDir);

    return m_LastFacing = finalDir;
}

glm::vec2 RangedAI::GetFaceDirection() {
    if (m_Freezed) {
        return glm::vec2(0.0f);
    }

    Status currentState = m_StateMachine.GetState();

    switch (currentState) {
        case Status::WANDER:
        case Status::FRIGHTENED:
            return m_LastFacing;

        case Status::STOPANDATTACK:
        case Status::PURSUIT:
            return this->GetFaceToTarget();
    }

    return glm::vec2(0.0f);
}

glm::vec2 RangedAI::GetAttackDirection() {
    if (m_Freezed) {
        return glm::vec2(0.0F);
    }

    Status currentState = m_StateMachine.GetState();

    switch (currentState) {
        case Status::WANDER:
        case Status::PURSUIT:
        case Status::FRIGHTENED:
            return glm::vec2(0.0F);

        case Status::STOPANDATTACK:
            float angle = m_Random.GetFloat() * AIConfig::PURSUIT_RADUIS_ANGLE;
            return this->ApplyRandomAngle(this->GetFaceToTarget(), angle);
    }

    return glm::vec2(0.0F);
}

void RangedAI::Update() {
    if (!m_Target || !m_Owner) {
        return;
    }

    m_StateMachine.SetState(
        this->GetNextState()
    );

    if ( this->NeedUpdate() ) {
        this->m_DesiredTranslation = this->CalculateDesiredTranslation();
        this->m_LastUpdateTime = Util::Time::GetElapsedTimeMs();
    }

    m_LastPosition = m_Owner->GetAbsoluteTranslation();
}

Status RangedAI::GetNextState() {
    float dist = GetDistanceToTarget();

    Status currentState = m_StateMachine.GetState();

    switch(currentState) {
        case Status::WANDER:
            if (dist < AIConfig::STATE_PERSUIT_MIN_RANGE) {
                return Status::FRIGHTENED;
            }

            return Status::WANDER;

        case Status::FRIGHTENED:
            if (m_StateMachine.GetCurrentStateDuration() > AIConfig::FRIGHTENED_WAIT_TIME) {
                return Status::PURSUIT;
            }

            return Status::FRIGHTENED;

        case Status::PURSUIT:
            if (dist < AIConfig::STATE_STOPANDATTACK_MAX_RANGE &&
                dist > AIConfig::STATE_STOPANDATTACK_MIN_RANGE && 
                m_StateMachine.GetCurrentStateDuration() > 3000
            ) {
                return Status::STOPANDATTACK;
            }

            return Status::PURSUIT;

        case Status::STOPANDATTACK:
            if (! (dist < AIConfig::STATE_STOPANDATTACK_MAX_RANGE && dist > AIConfig::STATE_STOPANDATTACK_MIN_RANGE) ) {
                return Status::PURSUIT;
            }

            if (m_StateMachine.GetCurrentStateDuration() > 4000) {
                return Status::PURSUIT;
            }
            
            return Status::STOPANDATTACK;
    }

    throw std::runtime_error("Invalid state in RangedAI::GetNextState()");
}

glm::vec2 RangedAI::CalculateDesiredTranslation() {
    Status currentState = m_StateMachine.GetState();

    const glm::vec2 mobPosition = this->m_Owner->GetAbsoluteTranslation();
    const glm::vec2 targetPosition = this->m_Target->GetAbsoluteTranslation();

    if (currentState == Status::WANDER) {
        glm::vec2 toTargetVec = targetPosition - mobPosition;

        toTargetVec = this->ApplyRandomAngle(toTargetVec, M_PI * 2.0F);
        toTargetVec = glm::normalize(toTargetVec);

        float distance = m_Random.GetFloat(
            this->GetDistanceToTarget() / 3.0F
        );

        return mobPosition + toTargetVec * distance;
    }

    if (currentState == Status::FRIGHTENED) {
        // Stand still if mob is frightened
        return this->m_Owner->GetAbsoluteTranslation();
    }

    if (currentState == Status::PURSUIT) {
        // Head toward to the player while pursuit mode
        glm::vec2 playerPosition = this->m_Target->GetAbsoluteTranslation();
        float rangeToPlayer = m_Random.GetFloat(AIConfig::STATE_STOPANDATTACK_MIN_RANGE, AIConfig::STATE_STOPANDATTACK_MAX_RANGE);

        glm::vec2 vector = glm::normalize( this->m_Owner->GetAbsoluteTranslation() - playerPosition);

        vector = this->ApplyRandomAngle(vector, M_PI);

        return playerPosition + vector * rangeToPlayer;
    }

    if (currentState == Status::STOPANDATTACK) {
        // float randAngle = m_Random.GetFloat() * AIConfig::PURSUIT_RADUIS_ANGLE;

        // const float radius = (AIConfig::ATTACK_MAXIMAL_RADIUS - AIConfig::ATTACK_MINIMAL_RADIUS);
        // float randRadius = AIConfig::ATTACK_MINIMAL_RADIUS + radius * m_Random.GetFloat();

        // this->m_DesiredTranslation = this->ApplyRandomAngle(-GetFaceToTarget(), randAngle);
        // this->m_DesiredTranslation = this->m_DesiredTranslation * randRadius;
        
        return mobPosition;
    }

    return mobPosition;
}

bool RangedAI::NeedUpdate() {
    if (m_Freezed) {
        return false;
    }

    Util::ms_t now = Util::Time::GetElapsedTimeMs();

    if (now - m_LastUpdateTime > 3500) {
        return true;
    }

    if (this->IsArriveDestinaton()) {
        return true;
    }

    return false;
}