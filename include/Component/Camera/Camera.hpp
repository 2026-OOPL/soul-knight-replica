#ifndef CAMERA_BASE_HPP
#define CAMERA_BASE_HPP

#include "Util/GameObject.hpp"
#include <Util/Transform.hpp>

class Camera {
public:
    Camera() {}

    Util::Transform GetTransform();

    // 計算一個遊戲物件在 Camera 視角下的渲染座標
    Util::Transform GetObjectTransform(Util::GameObject object);

protected:
    Util::Transform m_Transform; 
};

#endif