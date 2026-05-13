#include "Component/AI/MeleeAI.hpp"

#include <algorithm>

#include <glm/geometric.hpp>

#include "Component/Character/Character.hpp"
#include "Util/Time.hpp"

namespace {

constexpr float kMinAttackReleasePadding = 8.0F;

} // namespace

MeleeAI::MeleeAI(
    Character* owner,
    std::shared_ptr<Character> target,
    Collision::CollisionSystem* collision,
    MeleeAIConfig config
) : AI(owner, target, collision),
    m_Config(config) {
    this->m_Config.aggroRange = std::max(0.0F, this->m_Config.aggroRange);
    this->m_Config.attackRange = std::max(0.0F, this->m_Config.attackRange);
    this->m_Config.attackReleaseRange = std::max(
        this->m_Config.attackRange + kMinAttackReleasePadding,
        this->m_Config.attackReleaseRange
    );
    this->m_Config.pursuitUpdateIntervalMs =
        std::max(1.0F, this->m_Config.pursuitUpdateIntervalMs);
    this->m_Config.arrivalDistance = std::max(0.0F, this->m_Config.arrivalDistance);

    this->m_LastFacing = {1.0F, 0.0F};
    if (this->m_Owner != nullptr) {
        this->m_DesiredTranslation = this->m_Owner->GetAbsoluteTranslation();
        this->m_LastPosition = this->m_DesiredTranslation;
    }
}

glm::vec2 MeleeAI::GetMoveDirection() {
    if (this->m_Freezed ||
        this->m_Owner == nullptr ||
        this->m_Target == nullptr ||
        this->m_Target->IsDead()) {
        return {0.0F, 0.0F};
    }

    const Status currentState = this->m_StateMachine.GetState();
    if (currentState == Status::WANDER || currentState == Status::STOPANDATTACK) {
        return {0.0F, 0.0F};
    }

    const glm::vec2 ownerPosition = this->m_Owner->GetAbsoluteTranslation();
    const glm::vec2 desiredDirection = this->m_DesiredTranslation - ownerPosition;
    if (glm::length(desiredDirection) <= this->m_Config.arrivalDistance) {
        return {0.0F, 0.0F};
    }

    const glm::vec2 avoidedDirection = this->ApplyObstacleAvoidance(
        ownerPosition,
        this->NormalizeOrFallback(desiredDirection)
    );
    if (glm::length(avoidedDirection) <= 0.0001F) {
        return {0.0F, 0.0F};
    }

    this->m_LastFacing = glm::normalize(avoidedDirection);
    return this->m_LastFacing;
}

glm::vec2 MeleeAI::GetFaceDirection() {
    if (this->m_Freezed) {
        return {0.0F, 0.0F};
    }

    if (this->m_Owner == nullptr ||
        this->m_Target == nullptr ||
        this->m_Target->IsDead()) {
        return this->m_LastFacing;
    }

    const glm::vec2 toTarget =
        this->m_Target->GetAbsoluteTranslation() -
        this->m_Owner->GetAbsoluteTranslation();
    this->m_LastFacing = this->NormalizeOrFallback(toTarget);
    return this->m_LastFacing;
}

glm::vec2 MeleeAI::GetAttackDirection() {
    if (this->m_Freezed ||
        this->m_Owner == nullptr ||
        this->m_Target == nullptr ||
        this->m_Target->IsDead()) {
        return {0.0F, 0.0F};
    }

    if (this->m_StateMachine.GetState() != Status::STOPANDATTACK) {
        return {0.0F, 0.0F};
    }

    return this->GetFaceDirection();
}

void MeleeAI::Update() {
    if (this->m_Owner == nullptr) {
        return;
    }

    if (this->m_Freezed ||
        this->m_Target == nullptr ||
        this->m_Target->IsDead()) {
        this->m_DesiredTranslation = this->m_Owner->GetAbsoluteTranslation();
        this->m_LastPosition = this->m_DesiredTranslation;
        return;
    }

    this->m_StateMachine.SetState(this->GetNextState());

    if (this->NeedUpdate()) {
        this->m_DesiredTranslation = this->CalculateDesiredTranslation();
        this->m_LastUpdateTime = Util::Time::GetElapsedTimeMs();
    }

    this->m_LastPosition = this->m_Owner->GetAbsoluteTranslation();
}

Status MeleeAI::GetNextState() {
    const float distanceToTarget = this->GetDistanceToTarget();
    const Status currentState = this->m_StateMachine.GetState();

    switch (currentState) {
    case Status::WANDER:
    case Status::FRIGHTENED:
        if (distanceToTarget > this->m_Config.aggroRange) {
            return Status::WANDER;
        }
        return distanceToTarget <= this->m_Config.attackRange ?
            Status::STOPANDATTACK :
            Status::PURSUIT;

    case Status::PURSUIT:
        if (distanceToTarget > this->m_Config.aggroRange) {
            return Status::WANDER;
        }
        return distanceToTarget <= this->m_Config.attackRange ?
            Status::STOPANDATTACK :
            Status::PURSUIT;

    case Status::STOPANDATTACK:
        if (distanceToTarget > this->m_Config.aggroRange) {
            return Status::WANDER;
        }
        return distanceToTarget > this->m_Config.attackReleaseRange ?
            Status::PURSUIT :
            Status::STOPANDATTACK;
    }

    return Status::WANDER;
}

bool MeleeAI::NeedUpdate() {
    if (this->m_Freezed || this->m_Owner == nullptr) {
        return false;
    }

    const Util::ms_t now = Util::Time::GetElapsedTimeMs();
    if (now - this->m_LastUpdateTime >= this->m_Config.pursuitUpdateIntervalMs) {
        return true;
    }

    return glm::distance(
        this->m_Owner->GetAbsoluteTranslation(),
        this->m_DesiredTranslation
    ) <= this->m_Config.arrivalDistance;
}

glm::vec2 MeleeAI::CalculateDesiredTranslation() {
    if (this->m_Owner == nullptr ||
        this->m_Target == nullptr ||
        this->m_Target->IsDead()) {
        return this->m_Owner == nullptr ?
            glm::vec2{0.0F, 0.0F} :
            this->m_Owner->GetAbsoluteTranslation();
    }

    if (this->m_StateMachine.GetState() == Status::STOPANDATTACK) {
        return this->m_Owner->GetAbsoluteTranslation();
    }

    return this->m_Target->GetAbsoluteTranslation();
}

glm::vec2 MeleeAI::NormalizeOrFallback(const glm::vec2 &direction) const {
    if (glm::length(direction) <= 0.0001F) {
        return this->m_LastFacing;
    }

    return glm::normalize(direction);
}
