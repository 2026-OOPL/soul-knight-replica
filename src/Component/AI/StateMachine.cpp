#include "Component/AI/AI.hpp"
#include "Component/AI/StateMachine.hpp"

StateMachine::StateMachine(Status initialState) {
    m_CurrentState = initialState;

    int stateCount = static_cast<int>(Status::MAX);

    for (int i=0; i<=stateCount; i++ ) {
        m_StateStartTime.push_back(0);
    }
}

Status StateMachine::GetState() {
    return m_CurrentState;
}

void StateMachine::SetState(Status state) {
    if (state == m_CurrentState) {
        return;
    }
    
    m_CurrentState = state;

    int stateIndex = static_cast<int>(state);
    m_StateStartTime[stateIndex] = Util::Time::GetElapsedTimeMs();
}

Util::ms_t StateMachine::GetStateDuration(Status state) {
    int stateIndex = static_cast<int>(state);
    
    return Util::Time::GetElapsedTimeMs() - m_StateStartTime[stateIndex];
}

Util::ms_t StateMachine::GetCurrentStateDuration() {
    return this->GetStateDuration(this->m_CurrentState);
}