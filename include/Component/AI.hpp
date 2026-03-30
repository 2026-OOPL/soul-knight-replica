#ifndef AI_HPP
#define AI_HPP

#include <memory>

#include "Component/IStateful.hpp"
#include "Component/Player/Player.hpp"

namespace {

enum class Status {
    WANDER, 
    PURSUIT,
    STOPANDATTACK
};

}

class AI : public IStateful {
public:
    AI(std::weak_ptr<Character> target) {
      this->m_TracePlayer = target;
    };

    void Update() override;

protected:
    std::weak_ptr<Character> m_TracePlayer;
    Status m_Status = Status::WANDER;
};

#endif //