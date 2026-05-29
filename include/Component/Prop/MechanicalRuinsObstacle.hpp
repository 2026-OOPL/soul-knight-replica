#ifndef MECHANICAL_RUINS_OBSTACLE_HPP
#define MECHANICAL_RUINS_OBSTACLE_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Prop/IndestructibleObstacle.hpp"

class BaseRoom;

struct MechanicalRuinsObstacleConfig {
    std::shared_ptr<BaseRoom> owningRoom = nullptr;
};

class MechanicalRuinsBlock : public IndestructibleObstacle {
public:
    explicit MechanicalRuinsBlock(
        const glm::vec2 &coordinate,
        MechanicalRuinsObstacleConfig config = {}
    );
    ~MechanicalRuinsBlock() override = default;
};

class MechanicalRuinsFence : public IndestructibleObstacle {
public:
    explicit MechanicalRuinsFence(
        const glm::vec2 &coordinate,
        MechanicalRuinsObstacleConfig config = {}
    );
    ~MechanicalRuinsFence() override = default;
};

class MechanicalRuinsPillar : public IndestructibleObstacle {
public:
    explicit MechanicalRuinsPillar(
        const glm::vec2 &coordinate,
        MechanicalRuinsObstacleConfig config = {}
    );
    ~MechanicalRuinsPillar() override = default;
};

#endif
