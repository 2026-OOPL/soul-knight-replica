#ifndef CAMERA_BASE_HPP
#define CAMERA_BASE_HPP

#include <Util/Transform.hpp>
#include <memory>

#include "Component/IMapObject.hpp"

class Camera {
public:
    Camera() {

    }
    
    Camera(Util::Transform transform) {
        this->m_Transform = transform;
    }

    Util::Transform GetTransform();

    // 計算一個遊戲物件在 Camera 視角下的渲染座標
    virtual Util::Transform GetObjectTransform(std::shared_ptr<IMapObject> object) = 0;
    virtual bool GetVisibility(std::shared_ptr<IMapObject> object);

    glm::vec2 GetCameraSize();

    Util::Transform m_Transform;
protected:

};

#endif