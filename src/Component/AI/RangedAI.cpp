#include "Component/AI/RangedAI.hpp"

RangedAI::RangedAI(
    Character* owner,
    std::shared_ptr<Character> target,
    Collision::CollisionSystem* collision
) : AI(owner, target, collision) {

}