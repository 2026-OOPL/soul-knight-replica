#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <vector>

#include "Util/Time.hpp"

enum class Status;

class StateMachine {
public: 
    StateMachine(
        Status initialState
    );

    Status GetState();
    void SetState(Status state);

    Util::ms_t GetStateDuration(Status state);
    Util::ms_t GetCurrentStateDuration();

private:
    Status m_CurrentState;
    std::vector<Util::ms_t> m_StateStartTime;
};

#endif