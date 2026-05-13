#include "Component/AI/AI.hpp"

class RangedAI : public AI {
public:
    RangedAI(
        Character* owner,
        std::shared_ptr<Character> target,
        Collision::CollisionSystem* collision
    );

    ~RangedAI() override = default;

    glm::vec2 GetMoveDirection() override;
    glm::vec2 GetFaceDirection() override;
    glm::vec2 GetAttackDirection() override;

    void Update() override;

    
protected:
    Status GetNextState();
    bool NeedUpdate();

    glm::vec2 CalculateDesiredTranslation();
};
