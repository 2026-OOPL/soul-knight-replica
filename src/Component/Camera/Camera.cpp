#include <memory>
#include <stdexcept>

#include <glm/vec2.hpp>

#include "Core/Context.hpp"
#include "Component/IMapObject.hpp"
#include "Component/Camera/Camera.hpp"
#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"

glm::vec2 Camera::GetCameraSize() {
    auto context = Core::Context::GetInstance();
    return {context->GetWindowWidth(), context->GetWindowHeight()};
}

bool Camera::GetVisibilityByCamera(std::shared_ptr<IMapObject> object) {
    Util::Transform objectTransform = object->GetAbsoluteTransform();

    glm::vec2 objectSize = object->GetObjectSize();
    glm::vec2 cameraSize = this->GetCameraSize();

    glm::vec2 objectCooridinate = objectTransform.translation;
    glm::vec2 cameraCooridinate = this->m_Transform.translation;

    // Current window view port border in LTRB
    glm::vec4 windowBorder = {
        cameraCooridinate.x - cameraSize.x / 2,
        cameraCooridinate.y + cameraSize.y / 2,
        cameraCooridinate.x + cameraSize.x / 2,
        cameraCooridinate.y - cameraSize.y / 2,
    };

    // Current MapPiece render border in LTRB
    glm::vec4 mapBorder = {
        objectCooridinate.x - objectSize.x / 2,
        objectCooridinate.y + objectSize.y / 2,
        objectCooridinate.x + objectSize.x / 2,
        objectCooridinate.y - objectSize.y / 2,
    };

    const int margin = 10;
    
    float mLeft   = mapBorder.x - margin;
    float mTop    = mapBorder.y + margin;
    float mRight  = mapBorder.z + margin;
    float mBottom = mapBorder.w - margin;

    float wLeft   = windowBorder.x;
    float wTop    = windowBorder.y;
    float wRight  = windowBorder.z;
    float wBottom = windowBorder.w;

    bool isOutside = 
        (mRight  < wLeft  ) || // 整張在地圖左邊
        (mLeft   > wRight ) || // 整張在地圖右邊
        (mBottom > wTop   ) || // 整張在地圖上方 (Bottom 比 Window Top 還高)
        (mTop    < wBottom);   // 整張在地圖下方 (Top 比 Window Bottom 還低)

    return !isOutside;
}

void Camera::SetTransformByCamera(std::shared_ptr<Util::GameObject> object) {
    std::shared_ptr<IMapObject> mapObject = std::dynamic_pointer_cast<IMapObject>(object);
    
    if (mapObject == nullptr) {
        throw std::runtime_error("This object cannot be cast to IMapObject.");
    }

    const bool visible = this->GetVisibilityByCamera(mapObject);
    object->SetVisible(visible);

    if (visible) {
        object->m_Transform = this->GetTransformByCamera(mapObject);
    }
} 

void Camera::SetCooridinate(glm::vec2 coordinate) {
    this->m_Transform.translation = coordinate;
}

void Camera::SetScale(glm::vec2 scale) {
    this->m_Transform.scale = scale;
}
void Camera::SetRotation(double degree) {
    this->m_Transform.rotation = degree;
}

glm::vec2 Camera::GetCooridinate() {
    return this->m_Transform.translation;
}

glm::vec2 Camera::GetScale() {
    return this->m_Transform.scale;
}

float Camera::GetRotation() {
    return this->m_Transform.rotation;
}