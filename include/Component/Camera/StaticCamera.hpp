#ifndef STATIC_CAMERA_HPP
#define STATIC_CAMERA_HPP

#include "Component/Camera/Camera.hpp"
#include <glm/vec2.hpp>

class StaticCamera : public Camera {

    StaticCamera(Util::Transform transform) : Camera(transform) 
    {
            
    }

};

#endif