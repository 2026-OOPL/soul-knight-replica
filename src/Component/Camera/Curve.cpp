#include "Component/Camera/Curve.hpp"
#include <cmath>
#include <glm/exponential.hpp>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

float Curve::GetDistanceSquare(glm::vec2 start, glm::vec2 end) {
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    return (dx * dx) + (dy * dy); 
}

glm::vec2 LinearCurve::ApplyCurve(glm::vec2 _, glm::vec2 end) {
    return end;
}

glm::vec2 EaseOutQubicCurve::ApplyCurve(glm::vec2 start, glm::vec2 end) {
    const int MAGIC_NUMBER_1 = 10000;
    const int MAGIC_NUMBER_2 = 500;

    float distnace = this->GetDistanceSquare(start, end);

    if (distnace > MAGIC_NUMBER_1) {
        return end;
    }

    float dx = end.x - start.x;
    float dy = end.y - start.y;
    
    float px = std::abs(dx) / MAGIC_NUMBER_2;
    float mx = 1 - (1 - px) * (1 - px) * (1 - px);

    float py = std::abs(dy) / MAGIC_NUMBER_2;
    float my = 1 - (1 - py) * (1 - py) * (1 - py);

    my = (my < 0.05) ? 0 : my;
    mx = (mx < 0.05) ? 0 : mx;

    glm::vec2 direction = end - start;
    
    return start + glm::vec2(direction.x * mx, direction.y * my);
}