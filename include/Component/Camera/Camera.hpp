#ifndef CAMERA_BASE_HPP
#define CAMERA_BASE_HPP

#include <memory>
#include <glm/vec2.hpp>

#include <Util/Transform.hpp>

#include "Common/MapObject.hpp"
#include "Util/GameObject.hpp"

class Camera {
public:
    Camera() {}
    
    Camera(Util::Transform transform) {
        this->m_Transform = transform;
    }
    
    // 計算一個遊戲物件在 Camera 視角下的渲染座標
    virtual bool            GetVisibilityByCamera(std::shared_ptr<MapObject> object);
    virtual Util::Transform GetTransformByCamera(std::shared_ptr<MapObject> object) = 0;
    virtual void            SetTransformByCamera(std::shared_ptr<Util::GameObject> object);

    // Get the cooridinate where the camera needs to go to
    virtual glm::vec2 GetTargetCooridinate() = 0;

    Util::Transform GetTransform();
    glm::vec2 GetCameraSize();

    void SetCooridinate(glm::vec2 coordinate);
    void SetScale(glm::vec2 scale);
    void SetRotation(double degree);

    glm::vec2 GetCooridinate();
    glm::vec2 GetScale();
    float GetRotation();

protected:
    Util::Transform m_Transform;
};

#endif