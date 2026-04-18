#include "Component/AI/AI.hpp"

namespace RangedAIConfig {
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

class RangedAI : public AI {
public:
    RangedAI(
        Character* owner,
        std::shared_ptr<Character> target,
        Collision::CollisionSystem* collision
    );

};