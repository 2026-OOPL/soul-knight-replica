#ifndef I_ABSOLUTE_POSITION_HPP
#define I_ABSOLUTE_POSITION_HPP

#include <glm/vec2.hpp>

#include "Util/Transform.hpp"

class MapObject {
public:

    // Getter of the m_Transform in object
    glm::vec2 GetObjectScale() const;
    glm::vec2 GetObjectTranslation() const;
    float GetObjectRotation() const;

    // Setter of the absolute transform
    virtual void SetAbsoluteScale(glm::vec2 scale);
    virtual void SetAbsoluteTranslation(glm::vec2 transform);
    virtual void SetAbsoluteRotation(float degree);

    // Getter of the absolute transform
    virtual glm::vec2 GetAbsoluteTranslation() const;
    virtual glm::vec2 GetAbsoluteScale() const;
    virtual float GetAbsoluteRotation() const;

    virtual Util::Transform GetObjectTransform() const = 0; // Return the transformation of the object
    virtual Util::Transform GetAbsoluteTransform() const; // Return the absolute transformation of the object

protected:
    Util::Transform m_AbsoluteTransform;
};

#endif