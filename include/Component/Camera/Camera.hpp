#ifndef CAMERA_BASE_HPP
#define CAMERA_BASE_HPP

#include <Util/Transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

#include "Component/IMapObject.hpp"

class Camera {
public:
    Camera() {}
    
    Camera(Util::Transform transform) {
        this->m_Transform = transform;
    }

    Util::Transform GetTransform();

    // 計算一個遊戲物件在 Camera 視角下的渲染座標
    virtual Util::Transform GetTargetCooridinate(std::shared_ptr<IMapObject> object) = 0;
    virtual bool GetVisibility(std::shared_ptr<IMapObject> object);

    // Get the cooridinate where the camera needs to go to
    virtual glm::vec2 GetTargetCooridinate() = 0;

    glm::vec2 GetCameraSize();

    void SetCooridinate(glm::vec2 coordinate);
    void SetScale(glm::vec2 scale);
    void SetRotation(double degree);

    glm::vec2 GetCooridinate();
    glm::vec2 GetScale();
    double GetRotation();

private:
    Util::Transform m_Transform;
};

#endif