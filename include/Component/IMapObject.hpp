#ifndef I_ABSOLUTE_POSITION_HPP
#define I_ABSOLUTE_POSITION_HPP

#include "Util/Transform.hpp"

class IMapObject {
    virtual Util::Transform GetAbsolutePosition() = 0;

};

#endif