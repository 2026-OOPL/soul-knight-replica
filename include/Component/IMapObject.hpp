#ifndef I_ABSOLUTE_POSITION_HPP
#define I_ABSOLUTE_POSITION_HPP

#include <glm/vec2.hpp>

#include "Util/Transform.hpp"

class IMapObject {
public:

    virtual glm::vec2 GetAbsoluteScale() = 0;
    virtual Util::Transform GetAbsoluteTransform() = 0;
    

    virtual Util::Transform GetObjectTransform() = 0; // Return the transformation of the object
};

#endif