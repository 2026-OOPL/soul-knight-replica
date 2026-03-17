#ifndef I_ABSOLUTE_POSITION_HPP
#define I_ABSOLUTE_POSITION_HPP

#include "Util/Transform.hpp"
#include <glm/fwd.hpp>

class IMapObject {
public:
    virtual glm::vec2 GetObjectSize() = 0; 
    virtual glm::vec2 GetCooridinate() = 0; // Return the absolute cooridinate of the object on the map
    virtual Util::Transform GetTransform() = 0; // Return the transformation of the object
};

#endif