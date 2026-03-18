#ifndef CURVE_HPP
#define CURVE_HPP

#include <glm/vec2.hpp>

class Curve {
public:
    virtual glm::vec2 ApplyCurve(glm::vec2 start, glm::vec2 end) = 0;

    float GetDistanceSquare(glm::vec2 start, glm::vec2 end); 
};

class LinearCurve : public Curve {
    glm::vec2 ApplyCurve(glm::vec2 start, glm::vec2 end) override;
};

class EaseOutQubicCurve : public Curve {
    glm::vec2 ApplyCurve(glm::vec2 start, glm::vec2 end) override;
};

#endif